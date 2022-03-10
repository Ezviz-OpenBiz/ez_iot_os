#include "ezhal_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "ezlog.h"
#include "esp_err.h"

#include "string.h"
extern bool g_apstamode_flag;
static const char *TAG_WIFI = "T_WIFI";

static int g_wifi_start_flag = 0;
static int g_wifi_disconnect_flag = 0;

static const char *TAG_EVENT = "[WIFI EVENT]";

#define COUNTRY_CODE_LEN 3
#define ARRAY_SIZE_N(a) (sizeof(a) / sizeof((a)[0]))
/*8*/
static char us_country_code_table[][COUNTRY_CODE_LEN] = {
    "US", "CA", "MX", "PE", "TW", "SR", "VI"};

/*29*/
static char br_country_code_table[][COUNTRY_CODE_LEN] = {
    "BR", "CO", "BB", "CR", "EC", "UY", "BM", "DM", "DO", "GY", "HT", "TT",
    "NI", "HN", "PA", "VE", "PY", "BS", "FM", "GD", "GT", "JM", "PW", "SV",
    "VU", "CL", "AR", "IN", "PK"};

/*30*/
static char eu_country_code_table[][COUNTRY_CODE_LEN] = {
    "AT", "BE", "BG", "CY", "CZ", "DE", "DK", "EE", "ES", "FI", "FR", "GB",
    "GR", "HR", "HU", "IE", "IT", "LU", "MT", "NL", "PL", "PT", "RO", "SE",
    "SI", "SK", "TR", "LV", "LT", "EU"};

/*1*/
static char jp_country_code_table[][COUNTRY_CODE_LEN] = {
    "JP"};

/*46*/
static char other_country_code_table[][COUNTRY_CODE_LEN] = {
    "CN", "AF", "BF", "BH", "BI", "BN", "BO", "BT", "BW", "CD", "CF", "CG",
    "CI", "CK", "CU", "DJ", "DZ", "ER", "ET", "GH", "GM", "GN", "GQ", "GW",
    "KM", "KP", "LR", "MG", "MH", "MM", "MU", "NE", "NG", "NR", "NU", "PS",
    "RW", "SB", "SD", "SL", "SO", "ST", "SY", "TO", "TP", "TV"};

typedef enum
{
    eCN = 0,
    eEU,
    eUS,
    eJP,
    eBR,
} LIGHT_STATE_E;

extern char *get_product_country_code(void);
extern void set_product_country_code(char *CountryCode);
extern int Ezviz_Wifi_Config_Country_CodeMap(char *country_code);

int woal_is_us_country(char *country_code)
{
    unsigned char i = 0;

    for (i = 0; i < ARRAY_SIZE_N(us_country_code_table); i++)
    {
        if (!memcmp(country_code, us_country_code_table[i],
                    COUNTRY_CODE_LEN - 1))
        {
            ezlog_i(TAG_WIFI, "found region code=%s in US table.\n",
                     us_country_code_table[i]);

            return 1;
        }
    }
    return 0;
}

int woal_is_br_country(char *country_code)
{
    unsigned char i = 0;

    for (i = 0; i < ARRAY_SIZE_N(br_country_code_table); i++)
    {
        if (!memcmp(country_code, br_country_code_table[i], COUNTRY_CODE_LEN - 1))
        {
            ezlog_i(TAG_WIFI, "found region code=%s in BR table.\n", br_country_code_table[i]);
            return 1;
        }
    }
    return 0;
}

int woal_is_eu_country(char *country_code)
{
    unsigned char i = 0;

    for (i = 0; i < ARRAY_SIZE_N(eu_country_code_table); i++)
    {
        if (!memcmp(country_code, eu_country_code_table[i], COUNTRY_CODE_LEN - 1))
        {
            ezlog_i(TAG_WIFI, "found region code=%s in EU table\n", eu_country_code_table[i]);
            return 1;
        }
    }
    return 0;
}

int woal_is_jp_country(char *country_code)
{
    unsigned char i = 0;

    for (i = 0; i < ARRAY_SIZE_N(jp_country_code_table); i++)
    {
        if (!memcmp(country_code, jp_country_code_table[i], COUNTRY_CODE_LEN - 1))
        {
            printf("found region code=%s in JP table.\n", jp_country_code_table[i]);
            return 1;
        }
    }
    return 0;
}

int woal_is_other_country(char *country_code)
{
    unsigned char i = 0;

    for (i = 0; i < ARRAY_SIZE_N(other_country_code_table); i++)
    {
        if (!memcmp(country_code, other_country_code_table[i], COUNTRY_CODE_LEN - 1))
        {
            ezlog_i(TAG_WIFI, "found region code=%s in other table.\n", other_country_code_table[i]);
            return 1;
        }
    }
    return 0;
}

int Ezviz_Wifi_Config_Country_CodeMap(char *country_code)
{
    int8_t wifi_country_code = eCN;

    if (country_code == NULL)
    {
        printf("%s country_code is NULL !!! \n", __FUNCTION__);
        return -1;
    }

    if (woal_is_eu_country(country_code))
    {
        wifi_country_code = eEU;
    }
    else if (woal_is_us_country(country_code))
    {
        wifi_country_code = eUS;
    }
    else if (woal_is_jp_country(country_code))
    {
        wifi_country_code = eJP;
    }
    else if (woal_is_br_country(country_code))
    {
        wifi_country_code = eBR;
    }
    else
    {
        wifi_country_code = eCN;
    }
    return wifi_country_code;
    ezlog_i(TAG_WIFI, "config country code(%d)success.\n", wifi_country_code);
}

void Ezviz_Wifi_set_country_code(void)
{
    if (g_apstamode_flag)
    {
        ezlog_i(TAG_WIFI, "apsta mode,no need set country_code.\n");
        return;
    }
    wifi_country_t country = {"US", 1, 13, 20, WIFI_COUNTRY_POLICY_MANUAL};
    int iWifiCountryCodeMap = 0;
    iWifiCountryCodeMap = Ezviz_Wifi_Config_Country_CodeMap(get_product_country_code());
    switch (iWifiCountryCodeMap)
    {
    case eCN:
        country.nchan = 13;
        country.max_tx_power = 20;
        break;
    case eEU:
        country.nchan = 13;
        country.max_tx_power = 17;
        break;
    case eUS:
        country.nchan = 11;
        country.max_tx_power = 20;
        break;
    case eJP:
        country.nchan = 14;
        country.max_tx_power = 20;
        break;
    case eBR:
        country.nchan = 13;
        country.max_tx_power = 20;
        break;
    default:
        break;
    }

    ezlog_i(TAG_WIFI, "set countrycode(%s) end,wifi actual start %d and end %d !", get_product_country_code(), country.schan, country.nchan);
    //¡ä?o¡¥¨ºy?¨¢o???country.max_tx_power2?¨ºy
    esp_wifi_set_country(&country);
    // esp_wifi_set_max_tx_power(country.max_tx_power);
}


static EventGroupHandle_t g_wifi_event_group = NULL;
static ezhal_wifi_state_e g_wifi_connect_state = EZOS_WIFI_STATE_NOT_CONNECT;

static ez_bool_t g_wifi_init = false;

static ez_bool_t g_wifi_scan_start = false;
const int g_WIFI_SCAN_DONE_BIT = BIT0;

extern void wifi_status_cb(void *wifi_status);

static void wifi_disconnect_reason(uint8_t reason)
{
    switch (reason)
    {
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_AUTH_EXPIRE:
        ezlog_i(TAG_WIFI, "WIFI_REASON_PWD_ERROR(%d)", reason);
        g_wifi_connect_state = EZOS_WIFI_STATE_PASSWORD_ERROR;
        break;
    case WIFI_REASON_NO_AP_FOUND:
        ezlog_i(TAG_WIFI, "WIFI_REASON_NO_AP_FOUND(%d)", reason);
        g_wifi_connect_state = EZOS_WIFI_STATE_NO_AP_FOUND;
        break;
    case WIFI_REASON_ASSOC_FAIL:
        ezlog_i(TAG_WIFI, "WIFI_REASON_ASSOC_FAIL(%d)", reason);
        g_wifi_connect_state = EZOS_WIFI_STATE_UNKNOW;
        break;

    default:
        ezlog_e(TAG_WIFI, "unknow reason(%d)", reason);

        break;
    }
    return;
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    system_event_info_t *info = &event->event_info;

    ezlog_d(TAG_WIFI, "enter!!event->event_id=%d!!!", event->event_id);

    switch (event->event_id)
    {
    case SYSTEM_EVENT_AP_START:
        ezlog_i(TAG_WIFI, "AP_START SUCCESS!!!!!");
        break;
    case SYSTEM_EVENT_AP_STOP:
        ezlog_i(TAG_WIFI, "AP_STOP SUCCESS!!!!!");
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        ezlog_i(TAG_WIFI, "SYSTEM_EVENT_SCAN_DONE!!!!!");
        xEventGroupSetBits(g_wifi_event_group, g_WIFI_SCAN_DONE_BIT);
        break;
    case SYSTEM_EVENT_STA_START:
        ezlog_i(TAG_WIFI, "SYSTEM_EVENT_STA_START!!!!!");
        break;
    case SYSTEM_EVENT_STA_STOP:
        ezlog_i(TAG_WIFI, "SYSTEM_EVENT_STA_STOP!!!!!");
        g_wifi_connect_state = EZOS_WIFI_STATE_NOT_CONNECT;
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ezlog_i(TAG_WIFI, "SYSTEM_EVENT_STA_CONNECTED!!!!!");
        g_wifi_connect_state = EZOS_WIFI_STATE_CONNECT_SUCCESS;
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ezlog_i(TAG_WIFI, "got ip:%s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        wifi_status_cb(&event->event_info.got_ip.ip_info);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP8266 soft-AP */
        ezlog_w(TAG_WIFI, "AP_STA_CONNECTED");
        ezlog_i(TAG_WIFI, "station:" MACSTR " join, AID=%d",
                MAC2STR(event->event_info.sta_connected.mac),
                event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED: /**< ESP8266 soft-AP assign an IP to a connected station */
        ezlog_w(TAG_WIFI, "AP_STA_IPASSIGNED");
        g_wifi_connect_state = EZOS_WIFI_STATE_STA_CONNECTED;
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED: /**< a station disconnected from ESP8266 soft-AP */
        ezlog_w(TAG_WIFI, "AP_STA_DISCONNECTED");
        ezlog_i(TAG_WIFI, "station:" MACSTR "leave, AID=%d",
                MAC2STR(event->event_info.sta_disconnected.mac),
                event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ezlog_w(TAG_WIFI, "SYSTEM_EVENT_STA_DISCONNECTED");
        wifi_disconnect_reason(info->disconnected.reason);

        if (0 == g_wifi_disconnect_flag)
        {
            ezlog_i(TAG_EVENT, "Device DISCONNECTED,start a new connect!");
            esp_wifi_connect();
        }
        break;

    default:
        ezlog_e(TAG_WIFI, "unknow event id(%d)", event->event_id);
        break;
    }
    return ESP_OK;
}

int ezhal_wifi_init()
{
    ezlog_d(TAG_WIFI, "%s", __FUNCTION__);
    if (g_wifi_init)
    {
        ezlog_w(TAG_WIFI, "wifi inited.");
        return 0;
    }
    g_wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    g_wifi_init = true;
    return 0;
}

int ezhal_wifi_config(ezhal_wifi_mode_e wifi_mode)
{
    switch (wifi_mode)
    {
    case EZOS_WIFI_MODE_AP:
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        break;
    case EZOS_WIFI_MODE_STA:
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        break;
    case EZOS_WIFI_MODE_APSTA:
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        break;
    default:
        break;
    }

    return 0;
}

int ezhal_sta_connect(char *ssid, char *password)
{
    wifi_config_t wifi_config;
    esp_err_t ret = ESP_OK;

    ezlog_d(TAG_WIFI, "%s", __FUNCTION__);
    g_wifi_disconnect_flag = 0;
    wifi_mode_t wifi_mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&wifi_mode);
    if (WIFI_MODE_STA != wifi_mode && WIFI_MODE_APSTA != wifi_mode)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    }

    g_wifi_connect_state = EZOS_WIFI_STATE_NOT_CONNECT; //init connect state
    memset(&wifi_config, 0, sizeof(wifi_config));

    if ((ssid == NULL) || (strlen((char *)(ssid)) == 0))
    {
        ezlog_e(TAG_WIFI, "%s wifi config error, please check ssid!!!", __FUNCTION__);
        return -1;
    }

    if (strlen((char *)(ssid)) > 32)
    {
        ezlog_e(TAG_WIFI, "%s wifi config error, ssid is too long!!!!", __FUNCTION__);
        return -1;
    }
    else
    {
        memcpy(wifi_config.sta.ssid, ssid, 32);
    }

    if (NULL == password)
    {
        wifi_config.sta.threshold.authmode = EZOS_WIFI_OPEN;
        ezlog_i(TAG_WIFI, "connect to ap SSID:%.32s password: NULL", wifi_config.sta.ssid);
    }
    else
    {
        if (strlen((char *)(password)) > 64)
        {
            ezlog_e(TAG_WIFI, "%s wifi config error, pwd is too long!!!!", __FUNCTION__);
            return -1;
        }
        //wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        memcpy(wifi_config.ap.password, password, 64);
        ezlog_i(TAG_WIFI, "connect to ap SSID:%.32s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    }

    wifi_config.sta.listen_interval = 10;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    Ezviz_Wifi_set_country_code();
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    g_wifi_start_flag = 1;

    ret = esp_wifi_connect();
    if (ESP_OK != ret)
    {
        switch (ret)
        {
        case ESP_ERR_WIFI_NOT_INIT:
            ezlog_i(TAG_WIFI, "WiFi is not initialized by Ezviz_Wifi_Init! ret = %d", ret);
            break;
        case ESP_ERR_WIFI_CONN:
            ezlog_i(TAG_WIFI, "WiFi internal error, station or soft-AP control block wrong! ret = %d", ret);
            break;
        case ESP_ERR_WIFI_SSID:
            ezlog_i(TAG_WIFI, "SSID of AP which station connects is invalid! ret = %d", ret);
            break;
        default:
            ezlog_i(TAG_WIFI, "unknown connect state ret = %d!", ret);
            break;
        }
        g_wifi_connect_state = EZOS_WIFI_STATE_UNKNOW;
    }
    return 0;
}

int ezhal_sta_stop()
{
    ezlog_d(TAG_WIFI, "%s", __FUNCTION__);
    g_wifi_connect_state = EZOS_WIFI_STATE_NOT_CONNECT;
    g_wifi_disconnect_flag = 1;
    esp_wifi_disconnect();
    return 0;
}

int ezhal_ap_start(char *ssid, char *password, unsigned char auth_mode, unsigned char channel)
{
    wifi_config_t wifi_config;
    int ssid_len = 0, pwd_len = 0;

    ezlog_d(TAG_WIFI, "%s", __FUNCTION__);

    wifi_mode_t wifi_mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&wifi_mode);
    if (WIFI_MODE_AP != wifi_mode && WIFI_MODE_APSTA != wifi_mode)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    }

    memset(&wifi_config, 0, sizeof(wifi_config));

    if ((ssid == NULL) || (strlen((char *)(ssid)) == 0))
    {
        ezlog_e(TAG_WIFI, "%s wifi config  error, please check ssid!!!", __FUNCTION__);
        return -1;
    }

    if (NULL == password && 0 != auth_mode)
    {
        ezlog_e(TAG_WIFI, "%s wifi config error, please set open mode or set pwd!!!", __FUNCTION__);
        return -1;
    }

    ssid_len = strlen((char *)(ssid));

    if (ssid_len > 32)
    {
        ezlog_e(TAG_WIFI, "%s wifi config error, ssid is too long!!!!", __FUNCTION__);
        return -1;
    }

    if (0 != auth_mode)
    {
        pwd_len = strlen((char *)(password));
        if (pwd_len > 64)
        {
            ezlog_e(TAG_WIFI, "%s wifi config error, pwd is too long!!!!", __FUNCTION__);
            return -1;
        }

        memcpy(wifi_config.ap.password, password, 64);
    }

    memcpy(wifi_config.ap.ssid, ssid, 32);
    wifi_config.ap.ssid_len = ssid_len;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = auth_mode;
    wifi_config.ap.channel = channel;

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));

    ezlog_i(TAG_WIFI, "wifi_init_softap finished.SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);

    ESP_ERROR_CHECK(esp_wifi_start());
    g_wifi_start_flag = 1;

    return 0;
}

int ezhal_ap_stop()
{
    wifi_mode_t wifi_mode = WIFI_MODE_NULL;
    esp_err_t ret = ESP_OK;
    
    g_wifi_disconnect_flag = 1;
    esp_wifi_get_mode(&wifi_mode);

    switch (wifi_mode)
    {
    case WIFI_MODE_AP:
        ezlog_d(TAG_WIFI, "stop ap mode.");
        esp_wifi_disconnect();
        break;
    case WIFI_MODE_APSTA:
        ezlog_d(TAG_WIFI, "stop apsta mode.");
        esp_wifi_disconnect();
        esp_wifi_deauth_sta(0);
        break;
    default:
        ezlog_w(TAG_WIFI, "not ap or apsta mode, nothing to do");
        break;
    }

    return ret;
}

int ezhal_wifi_deinit()
{
    esp_wifi_stop();
    esp_wifi_deinit();
    if (g_wifi_event_group)
    {
        vEventGroupDelete(g_wifi_event_group);
        g_wifi_event_group = NULL;
    }
    return 0;
}

unsigned char ezhal_sta_get_scan_list(unsigned char max_ap_num, ezhal_wifi_list_t *ap_list)
{
    if (g_wifi_scan_start)
    {
        ezlog_w(TAG_WIFI, "wifi scan already started.");
        return 0;
    }
    g_wifi_scan_start = true;
    static wifi_scan_config_t scanConf =
        {
            .ssid = NULL,
            .bssid = NULL,
            .channel = 0,
            .show_hidden = true,
        };

    ezlog_d(TAG_WIFI, "%s enter!!", __FUNCTION__);

    if (max_ap_num == 0 || ap_list == NULL)
    {
        ezlog_e(TAG_WIFI, "!%s parameter erroe!", __FUNCTION__);
        g_wifi_scan_start = false;
        return 0;
    }

    if (g_wifi_start_flag != 1)
    {
        ESP_ERROR_CHECK(esp_wifi_start());
    }

    wifi_ap_record_t *tmp_list = (wifi_ap_record_t *)malloc(max_ap_num * sizeof(wifi_ap_record_t));
    if (NULL == tmp_list)
    {
        ezlog_e(TAG_WIFI, "malloc tmp_list failed.");
        return -1;
    }

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, true));
    xEventGroupWaitBits(g_wifi_event_group, g_WIFI_SCAN_DONE_BIT, 0, 1, portMAX_DELAY);
    ezlog_i(TAG_WIFI, "WIFI scan done");
    xEventGroupClearBits(g_wifi_event_group, g_WIFI_SCAN_DONE_BIT);

    ez_uint16_t tmp_num = max_ap_num;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&tmp_num, tmp_list));

    for (size_t i = 0; i < tmp_num; i++)
    {
        ezhal_wifi_list_t *ap_info = ap_list + i;
        wifi_ap_record_t *ap_scan_info = tmp_list + i;
        ap_info->authmode = ap_scan_info->authmode;
        ap_info->rssi = ap_scan_info->rssi;
        ap_info->channel = ap_scan_info->primary;
        memcpy(ap_info->bssid, ap_scan_info->bssid, sizeof(ap_info->bssid));
        strncpy(ap_info->ssid, (const char *)ap_scan_info->ssid, sizeof(ap_info->ssid) - 1);
    }
    free(tmp_list);

    g_wifi_scan_start = false;
    return tmp_num;
}

int ezhal_get_rssi(char *rssi)
{
    int8_t ret = -1;

    if (EZOS_WIFI_STATE_CONNECT_SUCCESS == g_wifi_connect_state)
    {
        wifi_ap_record_t wifi_param = {0};
        esp_wifi_sta_get_ap_info(&wifi_param);
        *rssi = wifi_param.rssi;

        ezlog_i(TAG_WIFI, "Get Rssi success! rssi = %d", *rssi);

        ret = 0;
    }
    else
    {
        ezlog_i(TAG_WIFI, "Wifi Disconnected, Rssi can't get.");
    }

    return ret;
}

ezhal_wifi_state_e ezhal_get_wifi_state()
{
    return g_wifi_connect_state;
}

void ezhal_set_country_code(char *CountryCode)
{
    set_product_country_code(CountryCode);
}

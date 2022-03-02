#include "config_implement.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include "config_type.h"
#include "ezlog.h"

#include "product_config.h"

//#include "fdb_def.h"
#include "kv_imp.h"

#define TAG_CONFIG "T_CONF"

typedef enum
{
    TYPE_NUM,
    TYPE_STRING,
} bulb_value_type_e;

typedef struct
{
    bulb_key_e key_e;
    char *key_v;
    bulb_value_type_e value_type;
    int bulb_default_value_num;
    char *bulb_default_value_str;
} bulb_kv_t;


static const bulb_kv_t g_bulb_kv_tab[] = {
    // ap key
    {AP_ENABLE, "ap_enable", TYPE_NUM, 0, ""},
    {FIRST_BOOT, "first_boot", TYPE_NUM, 1, ""},
    {AP_TIME_OUT, "ap_time_out", TYPE_NUM, 0, ""},
    {POWER_ON_NUM, "power_on_num", TYPE_NUM, 0, ""},

    {WIFI_SSID, "wifi_ssid", TYPE_STRING, 0, ""},
    {WIFI_PASSWORD, "wifi_pwd", TYPE_STRING, 0, ""},
    {WIFI_CC, "wifi_cc", TYPE_STRING, 0, ""},
    {WIFI_IP, "wifi_ip", TYPE_STRING, 0, ""},
    {WIFI_MASK, "wifi_mask", TYPE_STRING, 0, ""},
    {WIFI_GATEWAY, "wifi_gateway", TYPE_STRING, 0, ""},

    {BOOT_LABEL, "boot_lable", TYPE_STRING, 0, ""},
    {OTA_CODE, "ota_code", TYPE_NUM, REBOOT_NORMAL, ""},

    {LOG_LEVEL, "loglvl", TYPE_NUM, EZ_ELOG_LVL_WARN, ""},
    {LOG_REC_START, "LogRecStart", TYPE_NUM, 1, ""},

    {PT_PARA1, "para1", TYPE_NUM, 0, ""},
    {PT_PARA2, "para2", TYPE_NUM, 0, ""},

    {DAYLIGHT, "daylight", TYPE_NUM, 0, ""},
    {NTP_SERVER, "ntp_server", TYPE_STRING, 0, "time.ys7.com"},
    {TIMEZONE, "time_zone", TYPE_STRING, 0, "UTC+08:00"},
    {DATLIGHT_STR, "daylight_string", TYPE_STRING, 0, ""},

    {DOMAIN, "domain", TYPE_STRING, 0, ""},
    {DEVICE_ID, "device_id", TYPE_STRING, 0, ""},
    {USER_ID, "user_id", TYPE_STRING, 0, ""},
    {MASTERKEY, "masterkey", TYPE_STRING, 0, ""},

    {DEV_LIST, "dev_list", TYPE_STRING, 0, ""},

    // light things key
    {SWITCH, "Switch", TYPE_NUM, 1, ""},
    {COLORTEMPERATURE, "ColorTemperature", TYPE_NUM, 2700, ""},
    {BRIGHTNESS, "Brightness", TYPE_NUM, 100, ""},
    {COUNTDOWNCFG, "CountdownCfg", TYPE_STRING, 0, ""},
    {LIGHTSWITCHPLAN, "LightSwitchPlan", TYPE_STRING, 0, ""},

    {WORKMODE, "WrokMode", TYPE_STRING, 0, ""},
    {CUSTOMSCENECFG, "CustomSceneCfg", TYPE_STRING, 0, ""},
    {COUNTDOWN, "Countdown", TYPE_STRING, 0, ""},
    {COLORRGB, "ColorRgb", TYPE_STRING, 0, ""},
    {MUSICRHYTHM, "MusicRhythm", TYPE_STRING, 0, ""},
    {BIORHYTHM, "Biorhythm", TYPE_STRING, 0, ""},
    {WAKEUP, "WakeUp", TYPE_STRING, 0, ""},
    {HELPSLEEP, "HelpSleep", TYPE_STRING, 0, ""},
    {TIMEMGR, "TimeMgr", TYPE_STRING, 0, "{\"timeZone\": \"UTC+08:00\",\"tzCode\": 42,\"daylightSavingTime\": 0}"},
    {WIFISTATUS, "WifiStatus", TYPE_STRING, 0, ""},
};

int config_set_value(bulb_key_e key_e, void *value, int len)
{
    int ret = 0;

    ret = kv_raw_set(g_bulb_kv_tab[key_e].key_v, value, len);
    if (0 != ret)
    {
        ezlog_e(TAG_CONFIG, "set value failed: %s", g_bulb_kv_tab[key_e].key_v);
    }

    return ret;
}

int config_get_value(bulb_key_e key_e, void *value, ez_size_t *len)
{
    int ret = 0;

    ret = kv_raw_get(g_bulb_kv_tab[key_e].key_v, NULL, len);
    if (0 != ret || 0 > *len)
    {
        ezlog_e(TAG_CONFIG, "get length failed: %s, len: %d", g_bulb_kv_tab[key_e].key_v, *len);
        return -1;
    }

    if (NULL == value)
    {
        return 0;
    }
    
    ret = kv_raw_get(g_bulb_kv_tab[key_e].key_v, value, len);
    if (0 != ret)
    {
        ezlog_e(TAG_CONFIG, "get value failed: %s", g_bulb_kv_tab[key_e].key_v);
    }

    return ret;
}

int config_reset_factory()
{
    int ret = 0;

    int tab_size = sizeof(g_bulb_kv_tab) / sizeof(bulb_kv_t);

    for (int i = 0; i < tab_size; i++)
    {
        if (TYPE_NUM == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, &g_bulb_kv_tab[i].bulb_default_value_num, sizeof(int));
        }
        else if (TYPE_STRING == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, g_bulb_kv_tab[i].bulb_default_value_str, strlen(g_bulb_kv_tab[i].bulb_default_value_str));
        }

        if (0 != ret)
        {
            ezlog_e(TAG_CONFIG, "reset config failed. key: %s", g_bulb_kv_tab[i].key_v);
            break;
        }
    }

    // TODO read from product config

    return ret;
}

int config_reset_wifi()
{
    int ret = 0;

    for (size_t i = AP_ENABLE; i <= WIFI_GATEWAY; i++)
    {
        if (TYPE_NUM == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, &g_bulb_kv_tab[i].bulb_default_value_num, sizeof(int));
        }
        else if (TYPE_STRING == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, g_bulb_kv_tab[i].bulb_default_value_str, strlen(g_bulb_kv_tab[i].bulb_default_value_str));
        }

        if (0 != ret)
        {
            ezlog_e(TAG_CONFIG, "reset config failed. key: %s", g_bulb_kv_tab[i].key_v);
            break;
        }
    }

    int enable = 1;
    ret = kv_raw_set(g_bulb_kv_tab[AP_ENABLE].key_v, &enable, sizeof(enable));
    if (0 != ret)
    {
        ezlog_e(TAG_CONFIG, "reset config failed. key: %s", g_bulb_kv_tab[AP_ENABLE].key_v);
        return ret;
    }

    return ret;
}

int config_reset_time_zone(void)
{
    int ret = 0;
    for (size_t i = DAYLIGHT; i <= DATLIGHT_STR; i++)
    {
        if (TYPE_NUM == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, &g_bulb_kv_tab[i].bulb_default_value_num, sizeof(int));
        }
        else if (TYPE_STRING == g_bulb_kv_tab[i].value_type)
        {
            ret = kv_raw_set(g_bulb_kv_tab[i].key_v, g_bulb_kv_tab[i].bulb_default_value_str, strlen(g_bulb_kv_tab[i].bulb_default_value_str));
        }

        if (0 != ret)
        {
            ezlog_e(TAG_CONFIG, "reset time_zone failed. key: %s", g_bulb_kv_tab[i].key_v);
            break;
        }
    }
    return ret;
}

int config_reset_time_zone_str(void)
{
    int ret = 0;

    ret = kv_raw_set(g_bulb_kv_tab[TIMEMGR].key_v, g_bulb_kv_tab[TIMEMGR].bulb_default_value_str, strlen(g_bulb_kv_tab[TIMEMGR].bulb_default_value_str));
    if (0 != ret)
    {
        ezlog_e(TAG_CONFIG, "reset time_zone string failed. key: %s", g_bulb_kv_tab[TIMEMGR].key_v);
    }

    return ret;
}

int config_print()
{
    int ret = 0;

    int tab_size = sizeof(g_bulb_kv_tab) / sizeof(bulb_kv_t);

    int value = -100;
    ez_size_t len = 0;
    char value_str[64] = {0};
    for (int i = 0; i < tab_size; i++)
    {
        memset(value_str, 0, sizeof(value_str));
        if (TYPE_NUM == g_bulb_kv_tab[i].value_type)
        {
            len = sizeof(value);
            ret = kv_raw_get(g_bulb_kv_tab[i].key_v, &value, &len);
            ezlog_i(TAG_CONFIG, "key: %s, value: %d", g_bulb_kv_tab[i].key_v, value);
        }
        else if (TYPE_STRING == g_bulb_kv_tab[i].value_type)
        {
            len = sizeof(value_str);
            ret = kv_raw_get(g_bulb_kv_tab[i].key_v, value_str, &len);
            ezlog_i(TAG_CONFIG, "key: %s, value: %s", g_bulb_kv_tab[i].key_v, value_str);
        }

        if (0 != ret)
        {
            ezlog_e(TAG_CONFIG, "reset config failed. key: %s", g_bulb_kv_tab[i].key_v);
            break;
        }
    }

    return ret;
}

#ifdef BULB_VERSION
/**@fn		  
 * @brief		  除开网络，产测参数，服务器以外的信息
 * @param[in]  
 * @param[out] 
 * @return	  
 */
int generate_default_config_user()
{
	#if 0
	write_default_wifi_config();

	write_default_ap_config();

	write_default_server_config();

	write_default_product_test_config();  
	#endif
	
	// write_default_default_light_config();

	// write_default_share_light_config();

	// write_default_shadow_version();

	// write_default_time_zone_string_config();

	// write_default_time_zone_config();

	// write_default_scene_conf_config();

	// write_default_scene_config();

	return 0;
}

/**@fn		  
 * @brief		  除开产测参数，以外的信息,退货换到其他用户手里
 * @param[in]  
 * @param[out] 
 * @return	  
 */
int generate_default_config_user_factory()
{
	
	// write_default_wifi_config();

	// write_default_ap_config();

	// write_default_server_config();

	// #if 0
	// write_default_product_test_config();  
	// #endif
	
	// write_default_default_light_config();

	// write_default_share_light_config();

	// write_default_shadow_version();

	// write_default_time_zone_string_config();

	// write_default_time_zone_config();

	// write_default_scene_conf_config();

	// write_default_scene_config();

	return 0;
}

#endif
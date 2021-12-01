//#include "http_server.h"
#include "eztimer.h"

#include "ezconn.h"
#include "ezlog.h"
#include "ezos_thread.h"
#include "ezos_wifi.h"
#include "ezconn_adapter.h"
#include "ezos_libc.h"

ez_err_t ezconn_wifi_init()
{
    ezlog_w(TAG_AP, "wifi init");
    return ezos_wifi_init();
}

ez_err_t ezconn_sta_start(ez_int8_t *ssid, ez_int8_t *password)
{
    ezlog_w(TAG_AP, "station start");
    return ezos_sta_connect(ssid, password);
}

ez_err_t ezconn_sta_stop()
{
    ezlog_w(TAG_AP, "station stop");
    return ezos_sta_stop();
}

ez_err_t ezconn_ap_start(ezconn_ap_info_t *ap_info, ezconn_dev_info_t *dev_info, wifi_info_cb cb)
{
    ezlog_w(TAG_AP, "ap start");

    if (NULL == ap_info || NULL == dev_info || NULL == cb)
    {
        ezlog_e(TAG_AP, "%s param error.", __FUNCTION__);
        return EZCONN_ERRNO_INVALID_PARAM;
    }

    return ezconn_adatper_init(ap_info, dev_info, cb);

   
}
ez_err_t ezconn_ap_stop()
{
    ezlog_w(TAG_AP, "ap stop");
    ezconn_adapter_deinit();
    ezos_ap_stop();
    return 0;
}

ez_err_t ezconn_wifi_deinit()
{
    ezlog_w(TAG_AP, "wifi deinit");
    return ezos_wifi_deinit();
}

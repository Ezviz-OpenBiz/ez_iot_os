/*******************************************************************************
 * Copyright © 2017-2021 Ezviz Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 * 
 * Contributors:
 * XuRongjun (xurongjun@ezvizlife.com) ez iot hub unit-test case
 *******************************************************************************/

#include <stdlib.h>
#include <ezos.h>
#include <utest.h>
#include <ezlog.h>
#include <kv_imp.h>

#include "ez_iot_core_def.h"
#include "ez_iot_core.h"
#include "ez_iot_hub.h"

#include "ezlog.h"
#include "ezos_kv.h"
#include "ez_iot_tsl.h"
// =================================================
#define EZ_IOT_CLOUD_ENTRY_HOST "test15.ys7.com"
#define EZ_IOT_CLOUD_ENTRY_PORT 8666
// =================================================

// =================================================
#define EZ_IOT_DEV_AUTH_MODE 1
#define EZ_IOT_DEV_UUID "4LYV8SK7UKLBOUOVS6HXVX:EF229364D"
#define EZ_IOT_DEV_LICENSE "2QaaGDXFRMwPavSdojL5Ny"
#define EZ_IOT_DEV_PRODUCT_KEY "4LYV8SK7UKLBOUOVS6HXVX"
#define EZ_IOT_DEV_NAME "EF229364D"
#define EZ_IOT_DEV_DISPLAY_NAME "IOT_UTEST_DEV"
#define EZ_IOT_DEV_ID ""
#define EZ_IOT_DEV_FWVER "V1.2.0 build 201212"
// =================================================
//#define CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS 10

void ut_gateway_errcode();
void ut_gateway_rw();
void ut_gateway_update();
void ut_gateway_clean();
void ut_gateway_enum();
void ut_gateway_full();

static long global_init();

UTEST_TC_EXPORT(ut_gateway_errcode, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);
UTEST_TC_EXPORT(ut_gateway_rw, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);
UTEST_TC_EXPORT(ut_gateway_update, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);
UTEST_TC_EXPORT(ut_gateway_enum, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);
UTEST_TC_EXPORT(ut_gateway_clean, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);
UTEST_TC_EXPORT(ut_gateway_full, global_init, NULL, CONFIG_EZIOT_UNIT_TEST_CASE_TIEMOUT_SECONDS);

static int m_event_id = -1;
static int m_base_event_id = -1;
static int m_challenge_code = -1;
static ez_char_t m_add_result_sn[32] = {0};

static ez_int32_t ez_event_notice_func(ez_event_e event_type, ez_void_t *data, ez_int32_t len);
static int32_t hub_recv_event_cb(ez_subdev_event_e event_type, void *data, int len);

static int dev_event_waitfor(int event_id, int time_ms);
static int wait4add_result(ez_subdev_event_e event_type, ez_char_t *sn, int time_ms);

static ez_server_info_t m_lbs_addr = {EZ_IOT_CLOUD_ENTRY_HOST, EZ_IOT_CLOUD_ENTRY_PORT};

static ez_dev_info_t m_dev_info = {
    .auth_mode = EZ_IOT_DEV_AUTH_MODE,
    .dev_subserial = EZ_IOT_DEV_UUID,
    .dev_verification_code = EZ_IOT_DEV_LICENSE,
    .dev_firmwareversion = EZ_IOT_DEV_FWVER,
    .dev_type = EZ_IOT_DEV_PRODUCT_KEY,
    .dev_typedisplay = EZ_IOT_DEV_DISPLAY_NAME,
    .dev_mac = EZ_IOT_DEV_ID,
};


static ez_dev_info_t m_dev_info2 = {
    .auth_mode = EZ_IOT_DEV_AUTH_MODE,
    .dev_subserial = EZ_IOT_DEV_UUID,
    .dev_verification_code = EZ_IOT_DEV_LICENSE,
    .dev_firmwareversion = EZ_IOT_DEV_FWVER,
    .dev_type = EZ_IOT_DEV_PRODUCT_KEY,
    .dev_typedisplay = EZ_IOT_DEV_DISPLAY_NAME,
    .dev_mac = EZ_IOT_DEV_ID,
};

static ez_hub_callbacks_t m_hub_cbs = {hub_recv_event_cb};

static ez_kv_func_t g_kv_func = {
    .ezos_kv_init = kv_init,
    .ezos_kv_raw_set = kv_raw_set,
    .ezos_kv_raw_get = kv_raw_get,
    .ezos_kv_del = kv_del,
    .ezos_kv_del_by_prefix = kv_del_by_prefix,
    .ezos_kv_print = kv_print,
    .ezos_kv_deinit = kv_deinit,
};

static int m_add_result = -1;

static ez_char_t *m_test_subdev_sn1 = "ATOAUV0AY9S0";
static ez_char_t *m_test_subdev_ver1 = "V1.2.2 build 210521";
static ez_char_t *m_test_subdev_type1 = "TYPE_TEST1";
static ez_char_t *m_test_subdev_sn2 = "ATOAUV0AY9S0";
static ez_char_t *m_test_subdev_ver2 = "V1.2.2 build 210521";
static ez_char_t *m_test_subdev_type2 = "TYPE_TEST2";

#define  SUBDEV_TEST_AUTH_MODE   1
#define  SUBDEV_TEST_SN   "3A06F838D"
#define  SUBDEV_TEST_PID  "243671E149854925BF18DD"
#define  SUBDEV_TEST_LICENSE  "gomD7724KwATNkzH2cw1dC"
#define  SUBDEV_TEST_VERSION  "V1.2.2 build 210521"

#define  SUBDEV_TEST_AUTH_MODE2   1
#define  SUBDEV_TEST_SN2   "9DD82B1DB"
#define  SUBDEV_TEST_PID2  "243671E149854925BF18DD"
#define  SUBDEV_TEST_LICENSE2  "axXwFmcHWBWYHnEUG4RdJ8"
#define  SUBDEV_TEST_VERSION2  "V1.2.2 build 210521"

static ez_subdev_info_t m_subdev1 = {
        .auth_mode = SUBDEV_TEST_AUTH_MODE,            ///< 认证模式：0-SAP设备, 1-licence设备
        .subdev_type =  SUBDEV_TEST_PID, ///< 子设备型号(licence设备为productKey)
        .subdev_sn = SUBDEV_TEST_SN,   ///< 子设备序列号(licence设备为deviceName)
        .subdev_vcode = SUBDEV_TEST_LICENSE, ///< 子设备验证码(对应licence认证中deviceLicense)
        .subdev_ver = SUBDEV_TEST_VERSION, ///< 子设备固件版本号
        .subdev_uuid ="",///< 子设备局域部ID，用于直连或者mesh网络通讯，一般为mac地址
        .sta = 0                  ///< 在线状态：0-不在线，1-在线
        };

static ez_subdev_info_t m_subdev11 = {
        .auth_mode = SUBDEV_TEST_AUTH_MODE,            ///< 认证模式：0-SAP设备, 1-licence设备
        .subdev_type =  SUBDEV_TEST_PID, ///< 子设备型号(licence设备为productKey)
        .subdev_sn = SUBDEV_TEST_SN,   ///< 子设备序列号(licence设备为deviceName)
        .subdev_vcode = SUBDEV_TEST_LICENSE, ///< 子设备验证码(对应licence认证中deviceLicense)
        .subdev_ver = SUBDEV_TEST_VERSION, ///< 子设备固件版本号
        .subdev_uuid ="",///< 子设备局域部ID，用于直连或者mesh网络通讯，一般为mac地址
        .sta = 0                  ///< 在线状态：0-不在线，1-在线
        };
static ez_subdev_info_t m_subdev22 = {
        .auth_mode = SUBDEV_TEST_AUTH_MODE2,            ///< 认证模式：0-SAP设备, 1-licence设备
        .subdev_type =  SUBDEV_TEST_PID2, ///< 子设备型号(licence设备为productKey)
        .subdev_sn = SUBDEV_TEST_SN2,   ///< 子设备序列号(licence设备为deviceName)
        .subdev_vcode = SUBDEV_TEST_LICENSE2, ///< 子设备验证码(对应licence认证中deviceLicense)
        .subdev_ver = SUBDEV_TEST_VERSION2, ///< 子设备固件版本号
        .subdev_uuid ="",///< 子设备局域部ID，用于直连或者mesh网络通讯，一般为mac地址
        .sta = 0                  ///< 在线状态：0-不在线，1-在线
        };



ez_int32_t tsl_notice2(ez_tsl_event_e event_type, ez_void_t *data, ez_int32_t len)
{ 
    return 0;
}
static ez_int32_t tsl_things_action2dev(const ez_char_t *sn, const ez_tsl_rsc_t *rsc_info, const ez_tsl_key_t *key_info,
                                     const ez_tsl_value_t *value_in, ez_tsl_value_t *value_out)
{
    ez_int_t ret=0;

    return ret;

}

static ez_int32_t tsl_things_property2cloud(const ez_char_t *sn, const ez_tsl_rsc_t *rsc_info, const ez_tsl_key_t *key_info, ez_tsl_value_t *value_out)
{
    return -1;
}

static ez_int32_t tsl_things_property2dev(const ez_char_t *sn, const ez_tsl_rsc_t *rsc_info, const ez_tsl_key_t *key_info, const ez_tsl_value_t *value)
{
    return 1;
}


ez_int_t ez_tsl_init(void)
{
    ez_tsl_callbacks_t tsl_things_cbs = {tsl_notice2, tsl_things_action2dev, tsl_things_property2cloud, tsl_things_property2dev};
    return ez_iot_tsl_init(&tsl_things_cbs);
}

void ut_gateway_errcode()
{
    ez_byte_t devid[32] = {0};
    ez_subdev_info_t subdev_temp = {0};

    //sdk core not init
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_add(&m_subdev1));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_del(m_test_subdev_sn1));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_ver_update(m_test_subdev_sn1, m_test_subdev_ver2));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_status_update(m_test_subdev_sn1, false));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_subdev_query(m_test_subdev_sn1, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_subdev_next(&subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_clean());
    uassert_int_equal(EZ_HUB_ERR_NOT_INIT, ez_iot_hub_deinit());

    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid));
    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func));
    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_init(&m_lbs_addr, &m_dev_info, ez_event_notice_func));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(NULL));
    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_start());

    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_add(NULL));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_del(NULL));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_ver_update(NULL, m_test_subdev_ver2));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_ver_update(m_test_subdev_sn1, NULL));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_status_update(NULL, false));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_subdev_query(NULL, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_subdev_query(m_test_subdev_sn1, NULL));
    uassert_int_equal(EZ_HUB_ERR_PARAM_INVALID, ez_iot_hub_subdev_next(NULL));

    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_del(m_test_subdev_sn1));
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_ver_update(m_test_subdev_sn1, m_test_subdev_ver2));
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_status_update(m_test_subdev_sn1, false));
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_subdev_query(m_test_subdev_sn1, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_ENUM_END, ez_iot_hub_subdev_next(&subdev_temp));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
}

void ut_gateway_rw()
{
    ez_byte_t devid[32] = {0};

    ez_subdev_info_t subdev_temp = {0};

    ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid);
    ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_init(&m_lbs_addr, &m_dev_info2, ez_event_notice_func));

    ez_tsl_init();

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(&m_hub_cbs));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_start());
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev11));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_int_equal(0, memcmp((void *)&m_subdev11, (void *)&subdev_temp, sizeof(subdev_temp)));

    uassert_int_equal(EZ_HUB_ERR_SUBDEV_EXISTED, ez_iot_hub_add(&m_subdev11));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_del(m_subdev11.subdev_sn));

    memset((void *)&subdev_temp, 0, sizeof(subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
}

void ut_gateway_update()
{
    ez_subdev_info_t subdev_temp = {0};
    static ez_char_t *temp_subdev_ver = "V3.0.0 build 210118";
    static bool temp_subdev_sta = false;

    ez_byte_t devid[32] = {0};

    ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid);
    ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_init(&m_lbs_addr, &m_dev_info2, ez_event_notice_func));
    ez_tsl_init();

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(&m_hub_cbs));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_start());

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev11));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_int_equal(0, memcmp((void *)&m_subdev11, (void *)&subdev_temp, sizeof(subdev_temp)));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_ver_update(m_subdev11.subdev_sn, temp_subdev_ver));
    memset((void *)&subdev_temp, 0, sizeof(subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_int_equal(0, strncmp((char *)temp_subdev_ver, (char *)subdev_temp.subdev_ver, sizeof(subdev_temp.subdev_ver) - 1));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_status_update(m_subdev11.subdev_sn, temp_subdev_sta));
    memset((void *)&subdev_temp, 0, sizeof(subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_false(subdev_temp.sta);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_clean());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
}

void ut_gateway_clean()
{
    ez_subdev_info_t subdev_temp = {0};

    ez_byte_t devid[32] = {0};

    ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid);
    ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_init(&m_lbs_addr, &m_dev_info2, ez_event_notice_func));
    ez_tsl_init();
    
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(&m_hub_cbs));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_start());

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev11));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev22));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_clean());

    ezlog_i(TAG_AP,"%s %d", __FUNCTION__, __LINE__);

    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_subdev_query(m_subdev11.subdev_sn, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_SUBDEV_NOT_FOUND, ez_iot_hub_subdev_query(m_subdev22.subdev_sn, &subdev_temp));
    uassert_int_equal(EZ_HUB_ERR_ENUM_END, ez_iot_hub_subdev_next(&subdev_temp));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
}

void ut_gateway_enum()
{
    ez_subdev_info_t subdev_temp = {0};
    const int subdev_max = 64;
    int subdev_count = 0;

    ez_byte_t devid[32] = {0};

    ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid);
    ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func);

    ez_iot_core_init(&m_lbs_addr, &m_dev_info, ez_event_notice_func);
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(&m_hub_cbs));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_start());

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev11));
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&m_subdev22));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_next(&subdev_temp));
    uassert_int_equal(0, memcmp((void *)&m_subdev11, (void *)&subdev_temp, sizeof(subdev_temp)));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_subdev_next(&subdev_temp));
    uassert_int_equal(0, memcmp((void *)&m_subdev22, (void *)&subdev_temp, sizeof(subdev_temp)));
    uassert_int_equal(EZ_HUB_ERR_ENUM_END, ez_iot_hub_subdev_next(&subdev_temp));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_clean());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());
    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
}

void ut_gateway_full()
{
    ez_subdev_info_t subdev_temp = {0};
    const int subdev_max = 64;
    int i = 0;

    ez_byte_t devid[32] = {0};

    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_ctrl(EZ_CMD_DEVID_SET, (ez_void_t *)devid));
    uassert_int_equal(EZ_CORE_ERR_SUCC, ez_iot_core_ctrl(EZ_CMD_KVIMPL_SET, (ez_void_t *)&g_kv_func));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_init(&m_lbs_addr, &m_dev_info2, ez_event_notice_func));

    ez_tsl_init();

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_init(&m_hub_cbs));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_clean());

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_start());

;
    for (i = 0; i < 5; i++)
    {
        snprintf((char *)subdev_temp.subdev_sn, sizeof(subdev_temp.subdev_sn), "E11111111_%d", i);
        uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_add(&subdev_temp));
    }

    uassert_int_equal(EZ_HUB_ERR_OUT_OF_RANGE, ez_iot_hub_add(&m_subdev11));

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_clean());
        ezlog_e(TAG_APP, "%s,%d",__FUNCTION__, __LINE__);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_hub_deinit());
        ezlog_e(TAG_APP, "%s,%d",__FUNCTION__, __LINE__);

    uassert_int_equal(EZ_HUB_ERR_SUCC, ez_iot_core_stop());
    ez_iot_core_deinit();
          
}

static int dev_event_waitfor(int event_id, int time_ms)
{
    int ret = -1;
    int index = 0;
    m_event_id = -1;

    do
    {
        if (event_id == m_event_id)
        {
            ret = 0;
            break;
        }

        ezos_delay_ms(10);
        index += 10;
    } while (index < time_ms);

    return ret;
}

static ez_int32_t ez_event_notice_func(ez_event_e event_type, ez_void_t *data, ez_int32_t len)
{
    switch (event_type)
    {
    case EZ_EVENT_ONLINE:
        m_event_id = EZ_EVENT_ONLINE;
        break;
    case EZ_EVENT_OFFLINE:
        /* save devid */
        break;

    default:
        break;
    }

    return 0;
}

static int32_t hub_recv_event_cb(ez_subdev_event_e event_type, void *data, int len)
{
    ez_subdev_info_t *sub_dev_info = NULL;

    switch (event_type)
    {
    case EZ_EVENT_SUBDEV_ADD_SUCC:
        strncpy(m_add_result_sn, (char *)data, sizeof(m_add_result_sn) - 1);
        break;
    case EZ_EVENT_SUBDEV_ADD_FAIL:
        sub_dev_info = (ez_subdev_info_t *)data;
        strncpy(m_add_result_sn, (char *)sub_dev_info->subdev_sn, sizeof(m_add_result_sn) - 1);
        break;

    default:
        break;
    }

//    m_add_result = event_type;

    return 0;
}




static int wait4add_result(ez_subdev_event_e event_type, ez_char_t *sn, int time_ms)
{
    int ret = -1;
    int index = 0;
    m_add_result = -1;
    memset(m_add_result_sn, 0, sizeof(m_add_result_sn));

    do
    {
        if (event_type == m_add_result &&
            0 == strcmp(m_add_result_sn, sn))
        {
            ret = 0;
            break;
        }

        ezos_delay_ms(10);
        index += 10;
    } while (index < time_ms);

    return ret;
}

static long global_init()
{
    ezlog_init();
    ezlog_start();
    ezlog_filter_lvl(CONFIG_EZIOT_UNIT_TEST_SDK_LOGLVL);

    ezos_strncpy(m_dev_info.dev_typedisplay, CONFIG_EZIOT_UNIT_TEST_DEV_DISPLAY_NAME, sizeof(m_dev_info.dev_typedisplay) - 1);
    ezos_strncpy(m_dev_info.dev_firmwareversion, CONFIG_EZIOT_UNIT_TEST_DEV_FIRMWARE_VERSION, sizeof(m_dev_info.dev_firmwareversion) - 1);

#if defined(CONFIG_EZIOT_UNIT_TEST_DEV_AUTH_MODE_SAP)
    m_dev_info.auth_mode = 0;
    ezos_strncpy(m_dev_info.dev_type, CONFIG_EZIOT_UNIT_TEST_DEV_TYPE, sizeof(m_dev_info.dev_type) - 1);
    ezos_strncpy(m_dev_info.dev_subserial, CONFIG_EZIOT_UNIT_TEST_DEV_SERIAL_NUMBER, sizeof(m_dev_info.dev_subserial) - 1);
    ezos_strncpy(m_dev_info.dev_verification_code, CONFIG_EZIOT_UNIT_TEST_DEV_VERIFICATION_CODE, sizeof(m_dev_info.dev_verification_code) - 1);
#elif defined(CONFIG_EZIOT_UNIT_TEST_DEV_AUTH_MODE_LICENCE)
    m_dev_info.auth_mode = 1;
    ezos_strncpy(m_dev_info.dev_type, CONFIG_EZIOT_UNIT_TEST_DEV_PRODUCT_KEY, sizeof(m_dev_info.dev_type) - 1);
    ezos_snprintf(m_dev_info.dev_subserial, sizeof(m_dev_info.dev_subserial) - 1, "%s:%s", CONFIG_EZIOT_UNIT_TEST_DEV_PRODUCT_KEY, CONFIG_EZIOT_UNIT_TEST_DEV_NAME);
    ezos_strncpy(m_dev_info.dev_verification_code, CONFIG_EZIOT_UNIT_TEST_DEV_LICENSE, sizeof(m_dev_info.dev_verification_code) - 1);
#endif
/*
    m_subdev11.sta = true;
    strncpy((char *)m_subdev1.subdev_sn, (char *)m_test_subdev_sn1, sizeof(m_subdev1.subdev_sn) - 1);
    strncpy((char *)m_subdev1.subdev_ver, (char *)m_test_subdev_ver1, sizeof(m_subdev1.subdev_ver) - 1);
    strncpy((char *)m_subdev1.subdev_type, (char *)m_test_subdev_type1, sizeof(m_subdev1.subdev_type) - 1);

    m_subdev22.sta = true;
    strncpy((char *)m_subdev2.subdev_sn, (char *)m_test_subdev_sn2, sizeof(m_subdev2.subdev_sn) - 1);
    strncpy((char *)m_subdev2.subdev_ver, (char *)m_test_subdev_ver2, sizeof(m_subdev2.subdev_ver) - 1);
    strncpy((char *)m_subdev2.subdev_type, (char *)m_test_subdev_type2, sizeof(m_subdev2.subdev_type) - 1);
*/
    return 0;
}
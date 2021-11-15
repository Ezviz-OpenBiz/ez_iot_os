#include <mbedtls/platform.h>
#include <ezlog.h>
#include "ez_iot_core.h"
#include "ez_iot_core_lowlvl.h"
#include "ezdev_sdk_kernel_struct.h"
#include "ezdev_sdk_kernel_extend.h"
#include "ezdev_sdk_kernel_event.h"
#include "ezdev_sdk_kernel_access.h"
#include "ezdev_sdk_kernel_risk_control.h"
#include "das_transport.h"
#include "utils.h"

static ez_mutex_t g_mutex_lock;
extern EZDEV_SDK_UINT32 g_das_transport_seq;
extern ezdev_sdk_kernel g_ezdev_sdk_kernel;

EZDEV_SDK_KERNEL_EXTEND_INTERFACE
DAS_TRANSPORT_INTERFACE
EZDEV_SDK_KERNEL_EVENT_INTERFACE
EZDEV_SDK_KERNEL_ACCESS_INTERFACE
EZDEV_SDK_KERNEL_RISK_CONTROL_INTERFACE

static EZDEV_SDK_UINT32 genaral_seq()
{
    int seq = 0;
    ezos_mutex_lock(g_mutex_lock);
    seq = ++g_das_transport_seq;
    ezos_mutex_unlock(g_mutex_lock);
    return seq;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_init(const ez_server_info_t *psrv_info, const ez_dev_info_t *pdev_info,
                                           const ez_byte_t *devid, sdk_kernel_event_notice kernel_event_notice_cb)

{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    if (sdk_idle0 != g_ezdev_sdk_kernel.my_state)
    {
        goto done;
    }

    ezos_memset(&g_ezdev_sdk_kernel, 0, sizeof(g_ezdev_sdk_kernel));
    mbedtls_platform_set_calloc_free(ezos_calloc, ezos_free);

    /* init auth method */
    g_ezdev_sdk_kernel.dev_def_auth_type = sdk_dev_auth_protocol_ecdh;
    g_ezdev_sdk_kernel.dev_cur_auth_type = sdk_dev_auth_protocol_ecdh;
    g_ezdev_sdk_kernel.dev_last_auth_type = sdk_dev_auth_protocol_ecdh;
    g_ezdev_sdk_kernel.dev_auth_type_count = 1;
    g_ezdev_sdk_kernel.dev_auth_type_group[0] = sdk_dev_auth_protocol_ecdh;
    g_ezdev_sdk_kernel.das_keepalive_interval = ezdev_sdk_das_default_keepaliveinterval;

    /* set server info */
    ezos_strncpy(g_ezdev_sdk_kernel.server_info.server_name, (char *)psrv_info->host, ezdev_sdk_name_len - 1);
    g_ezdev_sdk_kernel.server_info.server_port = psrv_info->port;

    /* set devinfo */
    g_ezdev_sdk_kernel.dev_info.dev_auth_mode = pdev_info->auth_mode;
    g_ezdev_sdk_kernel.dev_info.dev_status = 1;
    g_ezdev_sdk_kernel.dev_info.dev_oeminfo = 0;
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_subserial, (char *)pdev_info->dev_subserial, sizeof(g_ezdev_sdk_kernel.dev_info.dev_subserial) - 1);
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_verification_code, (char *)pdev_info->dev_verification_code, sizeof(g_ezdev_sdk_kernel.dev_info.dev_verification_code) - 1);
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_firmwareversion, (char *)pdev_info->dev_firmwareversion, sizeof(g_ezdev_sdk_kernel.dev_info.dev_firmwareversion) - 1);
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_type, (char *)pdev_info->dev_type, sizeof(g_ezdev_sdk_kernel.dev_info.dev_type) - 1);
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_typedisplay, (char *)pdev_info->dev_typedisplay, sizeof(g_ezdev_sdk_kernel.dev_info.dev_typedisplay) - 1);
    ezos_strncpy(g_ezdev_sdk_kernel.dev_info.dev_mac, (char *)pdev_info->dev_mac, sizeof(g_ezdev_sdk_kernel.dev_info.dev_mac) - 1);

    ezos_strncpy((char *)g_ezdev_sdk_kernel.dev_id, (char *)devid, sizeof(g_ezdev_sdk_kernel.dev_id));
    ez_int32_t key_len = sizeof(g_ezdev_sdk_kernel.master_key);
    CHECK_COND_DONE(!ezos_kv_raw_get(EZ_KV_DEFALUT_KEY_MASTERKEY, g_ezdev_sdk_kernel.master_key, &key_len), EZ_ERR_STORAGE);

    /* 初始化链接状态 */
    g_ezdev_sdk_kernel.lbs_redirect_times = 0;
    g_ezdev_sdk_kernel.das_retry_times = 0;
    g_ezdev_sdk_kernel.entr_state = sdk_entrance_normal;
    g_ezdev_sdk_kernel.my_state = sdk_idle0;
    g_ezdev_sdk_kernel.cnt_state = sdk_cnt_unredirect;
    g_ezdev_sdk_kernel.access_risk = sdk_no_risk_control;
    g_ezdev_sdk_kernel.reg_mode = 1;
    ezos_memset(&g_ezdev_sdk_kernel.cnt_state_timer, 0, sizeof(g_ezdev_sdk_kernel.cnt_state_timer));

    /* 初始化领域和公共领域 */
    extend_init(kernel_event_notice_cb);

    /* 初始化MQTT和消息队列 */
    das_object_init(&g_ezdev_sdk_kernel);
    g_mutex_lock = ezos_mutex_create();
    g_das_transport_seq = 0;
    g_ezdev_sdk_kernel.my_state = sdk_idle;

done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_start()
{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    CHECK_COND_DONE(sdk_idle0 == g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_INIT);
    CHECK_COND_DONE(sdk_start == g_ezdev_sdk_kernel.my_state, EZ_ERR_GENERAL);

    g_ezdev_sdk_kernel.my_state = sdk_start;
    broadcast_user_start();

done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_stop()
{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    CHECK_COND_DONE(sdk_idle0 == g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_INIT);
    CHECK_COND_DONE(sdk_idle == g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_READY);
    CHECK_COND_DONE(sdk_stop == g_ezdev_sdk_kernel.my_state, EZ_ERR_GENERAL);

    g_ezdev_sdk_kernel.my_state = sdk_stop;
    access_stop_yield(&g_ezdev_sdk_kernel);

done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_fini()
{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    CHECK_COND_DONE(sdk_idle0 == g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_INIT);
    CHECK_COND_DONE(sdk_start == g_ezdev_sdk_kernel.my_state, EZ_ERR_GENERAL);

    das_object_fini(&g_ezdev_sdk_kernel);
    extend_fini();

    if (g_mutex_lock)
    {
        ezos_mutex_destory(g_mutex_lock);
        g_mutex_lock = NULL;
    }

    ezos_memset(&g_ezdev_sdk_kernel, 0, sizeof(g_ezdev_sdk_kernel));
done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_yield()
{
    ez_err_t rv = EZ_ERR_SUCC;
    CHECK_COND_DONE(sdk_start != g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_READY);
    rv = mkiE2ezE(access_server_yield(&g_ezdev_sdk_kernel));

    ezlog_v(TAG_CORE, "yield rv:%d", rv);
done:
    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_yield_user()
{
    ez_err_t rv = EZ_ERR_SUCC;
    CHECK_COND_DONE(sdk_start != g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_READY);
    rv = mkiE2ezE(extend_yield(&g_ezdev_sdk_kernel));

    ezlog_v(TAG_CORE, "yield_user rv:%d", rv);
done:
    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_extend_load(const ez_kernel_extend_t *external_extend)
{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    if (sdk_idle != g_ezdev_sdk_kernel.my_state && sdk_start != g_ezdev_sdk_kernel.my_state)
    {
        rv = EZ_ERR_NOT_INIT;
        goto done;
    }

    CHECK_COND_DONE(!external_extend, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!external_extend->ezdev_sdk_kernel_extend_data_route, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!external_extend->ezdev_sdk_kernel_extend_start, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!external_extend->ezdev_sdk_kernel_extend_stop, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!extend_load(external_extend), EZ_ERR_GENERAL);

done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_send(ez_kernel_pubmsg_t *pubmsg)
{
    FUNC_IN();

    ezdev_sdk_kernel_pubmsg_exchange *new_pubmsg_exchange = NULL;
    ez_err_t rv = EZ_ERR_SUCC;
    char cRiskResult = 0;

    CHECK_COND_DONE(sdk_start != g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_INIT);
    CHECK_COND_DONE(!pubmsg, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!pubmsg->msg_body, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!pubmsg->msg_body_len, EZ_ERR_PARAM_INVALID);

    ezlog_i(TAG_CORE, "domain:%d ,cmd:%d, seq:%d, len:%d, data:%s", pubmsg->msg_domain_id,
            pubmsg->msg_command_id, pubmsg->msg_seq, pubmsg->msg_body_len, pubmsg->msg_body);

    CHECK_COND_DONE(pubmsg->msg_body_len > ezdev_sdk_send_buf_max, EZ_ERR_OUT_RANGE);

    cRiskResult = check_cmd_risk_control(&g_ezdev_sdk_kernel, pubmsg->msg_domain_id, pubmsg->msg_command_id);
    CHECK_COND_DONE(1 == cRiskResult, EZ_ERR_NO_EXTEND);
    CHECK_COND_DONE((2 == cRiskResult || 3 == cRiskResult), EZ_ERR_RISK_CRTL);

    new_pubmsg_exchange = (ezdev_sdk_kernel_pubmsg_exchange *)ezos_malloc(sizeof(ezdev_sdk_kernel_pubmsg_exchange));
    CHECK_COND_DONE(!new_pubmsg_exchange, EZ_ERR_MEMORY);

    ezos_memset(new_pubmsg_exchange, 0, sizeof(ezdev_sdk_kernel_pubmsg_exchange));
    ezos_strncpy(new_pubmsg_exchange->msg_conntext.command_ver, pubmsg->command_ver, version_max_len - 1);
    new_pubmsg_exchange->msg_conntext.msg_response = pubmsg->msg_response;
    new_pubmsg_exchange->msg_conntext.msg_qos = pubmsg->msg_qos;
    new_pubmsg_exchange->msg_conntext.msg_seq = pubmsg->msg_seq;
    new_pubmsg_exchange->msg_conntext.msg_domain_id = pubmsg->msg_domain_id;
    new_pubmsg_exchange->msg_conntext.msg_command_id = pubmsg->msg_command_id;

    new_pubmsg_exchange->msg_conntext.msg_body = (unsigned char *)ezos_malloc(pubmsg->msg_body_len);
    CHECK_COND_DONE(!new_pubmsg_exchange->msg_conntext.msg_body, EZ_ERR_MEMORY);

    ezos_memset(new_pubmsg_exchange->msg_conntext.msg_body, 0, pubmsg->msg_body_len);
    new_pubmsg_exchange->msg_conntext.msg_body_len = pubmsg->msg_body_len;
    ezos_memcpy(new_pubmsg_exchange->msg_conntext.msg_body, pubmsg->msg_body, pubmsg->msg_body_len);
    new_pubmsg_exchange->max_send_count = ezdev_sdk_max_publish_count;

    if (pubmsg->msg_response == 0)
    {
        pubmsg->msg_seq = genaral_seq();
        new_pubmsg_exchange->msg_conntext.msg_seq = pubmsg->msg_seq;
    }

    if (NULL != pubmsg->externel_ctx && 0 != pubmsg->externel_ctx_len)
    {
        new_pubmsg_exchange->msg_conntext.externel_ctx = (unsigned char *)ezos_malloc(pubmsg->externel_ctx_len);
        CHECK_COND_DONE(!new_pubmsg_exchange->msg_conntext.externel_ctx, EZ_ERR_MEMORY);

        ezos_memcpy(new_pubmsg_exchange->msg_conntext.externel_ctx, pubmsg->externel_ctx, pubmsg->externel_ctx_len);
        new_pubmsg_exchange->msg_conntext.externel_ctx_len = pubmsg->externel_ctx_len;
    }

    CHECK_COND_DONE(das_send_pubmsg_async(&g_ezdev_sdk_kernel, new_pubmsg_exchange), EZ_ERR_MEMORY);
done:

    if (rv != EZ_ERR_SUCC && NULL != new_pubmsg_exchange)
    {
        if (NULL != new_pubmsg_exchange->msg_conntext.msg_body)
        {
            ezos_free(new_pubmsg_exchange->msg_conntext.msg_body);
        }

        if (NULL != new_pubmsg_exchange->msg_conntext.externel_ctx)
        {
            ezos_free(new_pubmsg_exchange->msg_conntext.externel_ctx);
        }

        ezos_free(new_pubmsg_exchange);
    }

    FUNC_OUT();
    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_extend_load_v3(const ez_kernel_extend_v3_t *external_extend)
{
    FUNC_IN();
    ez_err_t rv = EZ_ERR_SUCC;

    if (sdk_idle != g_ezdev_sdk_kernel.my_state && sdk_start != g_ezdev_sdk_kernel.my_state)
    {
        rv = EZ_ERR_NOT_INIT;
        goto done;
    }

    CHECK_COND_DONE(!external_extend, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!external_extend->ez_kernel_data_route, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!external_extend->ez_kernel_event_route, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!ezos_strlen(external_extend->module), EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!extend_load_v3(external_extend), EZ_ERR_GENERAL);
    g_ezdev_sdk_kernel.v3_reg_status = sdk_v3_reged;

done:
    FUNC_OUT();

    return rv;
}

EZOS_API ez_err_t EZOS_CALL ez_kernel_send_v3(ez_kernel_pubmsg_v3_t *pubmsg)
{
    FUNC_IN();

    ezdev_sdk_kernel_pubmsg_exchange_v3 *new_pubmsg_exchange = NULL;
    ez_err_t rv = EZ_ERR_SUCC;

    CHECK_COND_DONE(sdk_start != g_ezdev_sdk_kernel.my_state, EZ_ERR_NOT_INIT);
    CHECK_COND_DONE(!pubmsg, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!pubmsg->msg_body, EZ_ERR_PARAM_INVALID);
    CHECK_COND_DONE(!pubmsg->msg_body_len, EZ_ERR_PARAM_INVALID);

    ezlog_i(TAG_CORE, "module:%s, resource_type:%s,msg_type:%s, method:%s, ext_msg:%s, seq:%d, len:%d, string:%s\n", pubmsg->module,
            pubmsg->resource_type, pubmsg->msg_type, pubmsg->method, pubmsg->ext_msg, pubmsg->msg_seq, pubmsg->msg_body_len, pubmsg->msg_body);

    CHECK_COND_DONE(pubmsg->msg_body_len > ezdev_sdk_send_buf_max, EZ_ERR_OUT_RANGE);

    new_pubmsg_exchange = (ezdev_sdk_kernel_pubmsg_exchange_v3 *)ezos_malloc(sizeof(ezdev_sdk_kernel_pubmsg_exchange_v3));
    CHECK_COND_DONE(!new_pubmsg_exchange, EZ_ERR_MEMORY);

    ezos_memset(new_pubmsg_exchange, 0, sizeof(ezdev_sdk_kernel_pubmsg_exchange_v3));
    new_pubmsg_exchange->msg_conntext_v3.msg_qos = pubmsg->msg_qos;
    new_pubmsg_exchange->msg_conntext_v3.msg_seq = pubmsg->msg_seq;
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.module, pubmsg->module, ezdev_sdk_module_name_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.resource_id, pubmsg->resource_id, ezdev_sdk_resource_id_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.resource_type, pubmsg->resource_type, ezdev_sdk_resource_type_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.msg_type, pubmsg->msg_type, ezdev_sdk_msg_type_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.method, pubmsg->method, ezdev_sdk_method_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.sub_serial, pubmsg->sub_serial, ezdev_sdk_max_serial_len - 1);
    ezos_strncpy(new_pubmsg_exchange->msg_conntext_v3.ext_msg, pubmsg->ext_msg, ezdev_sdk_ext_msg_len - 1);

    if (0 == pubmsg->msg_response)
    {
        pubmsg->msg_seq = genaral_seq();
    }

    new_pubmsg_exchange->msg_conntext_v3.msg_body = (unsigned char *)ezos_malloc(pubmsg->msg_body_len);
    CHECK_COND_DONE(!new_pubmsg_exchange->msg_conntext_v3.msg_body, EZ_ERR_MEMORY);

    ezos_memset(new_pubmsg_exchange->msg_conntext_v3.msg_body, 0, pubmsg->msg_body_len);
    new_pubmsg_exchange->msg_conntext_v3.msg_body_len = pubmsg->msg_body_len;
    ezos_memcpy(new_pubmsg_exchange->msg_conntext_v3.msg_body, pubmsg->msg_body, pubmsg->msg_body_len);
    new_pubmsg_exchange->max_send_count = ezdev_sdk_max_publish_count;
    new_pubmsg_exchange->msg_conntext_v3.msg_seq = pubmsg->msg_seq;

    CHECK_COND_DONE(das_send_pubmsg_async_v3(&g_ezdev_sdk_kernel, new_pubmsg_exchange), EZ_ERR_MEMORY);
done:

    if (rv != EZ_ERR_SUCC && NULL != new_pubmsg_exchange)
    {
        if (NULL != new_pubmsg_exchange->msg_conntext_v3.msg_body)
        {
            ezos_free(new_pubmsg_exchange->msg_conntext_v3.msg_body);
        }

        ezos_free(new_pubmsg_exchange);
    }

    FUNC_OUT();
    return rv;
}

EZOS_API const ez_char_t *EZOS_CALL ez_kernel_getdevinfo_bykey(const ez_char_t *key)
{
    static const ez_char_t *g_default_value = "invalidkey";

    if (ezos_strcmp(key, "dev_subserial") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_subserial;
    }
    else if (ezos_strcmp(key, "dev_serial") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_serial;
    }
    else if (ezos_strcmp(key, "dev_firmwareversion") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_firmwareversion;
    }
    else if (ezos_strcmp(key, "dev_type") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_type;
    }
    else if (ezos_strcmp(key, "dev_typedisplay") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_typedisplay;
    }
    else if (ezos_strcmp(key, "dev_mac") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_mac;
    }
    else if (ezos_strcmp(key, "dev_nickname") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_nickname;
    }
    else if (ezos_strcmp(key, "dev_firmwareidentificationcode") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_firmwareidentificationcode;
    }
    else if (ezos_strcmp(key, "dev_verification_code") == 0)
    {
        return g_ezdev_sdk_kernel.dev_info.dev_verification_code;
    }
    else
    {
        return g_default_value;
    }
}
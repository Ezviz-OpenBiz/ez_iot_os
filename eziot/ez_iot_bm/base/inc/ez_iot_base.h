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
 * XuRongjun (xurongjun@ezvizlife.com)
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-15     xurongjun    first version 
 *******************************************************************************/

#ifndef _EZ_IOT_BASE_H_
#define _EZ_IOT_BASE_H_

#include <ezos.h>

#define BASE_MODULE_ERRNO_BASE 0x00010000

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        EZ_BASE_ERR_SUCC = 0x00,                                        ///< Success
        EZ_BASE_ERR_NOT_INIT = BASE_MODULE_ERRNO_BASE + 0x01,          ///< The module is not initialized
        EZ_BASE_ERR_NOT_READY = BASE_MODULE_ERRNO_BASE + 0x02,         ///< The sdk core module is not started
        EZ_BASE_ERR_PARAM_INVALID = BASE_MODULE_ERRNO_BASE + 0x03,     ///< The input parameters is illegal, it may be that some parameters can not be null or out of range
        EZ_BASE_ERR_GENERAL = BASE_MODULE_ERRNO_BASE + 0x04,           ///< Unknown error
        EZ_BASE_ERR_MEMORY = BASE_MODULE_ERRNO_BASE + 0x05,            ///< Out of memory
    } ez_base_err_e;

    typedef struct
    {
        ez_char_t user_id[32];   ///< 绑定设备的用户ID
    } ez_bind_info_t;

    typedef struct
    {
        ez_int32_t challenge_code;  ///< 挑战码
        ez_int32_t validity_period; ///< 有效期(秒)
    } ez_bind_challenge_t;

    typedef enum
    {
        EZ_EVENT_BINDING,           ///< 设备已绑定, msg data: \c ez_bind_info_t
        EZ_EVENT_UNBINDING,         ///< 设备未绑定, msg data: null
        EZ_EVENT_BINDING_CHALLENGE, ///< 绑定挑战, msg data: \c ez_bind_challenge_t
    } ez_base_event_e;

    typedef ez_int32_t (*ez_base_notice)(ez_base_event_e event_type, ez_void_t *data, ez_int32_t len);

    /**
     * @brief 
     * 
     * @param pfunc 
     * @return ez_err_t
     */
    EZOS_API ez_err_t ez_iot_base_init(const ez_base_notice pfunc);

    /**
     * @brief 查询设备绑定关系;通过ez_base_notice回调接收结果。
     * 
     * @return ez_err_t
     */
    EZOS_API ez_err_t ez_iot_base_bind_query();

    /**
     * @brief 近场添加, 通常用于wifi、蓝牙通讯协议设备。
     *        1.通过近场通讯技术，设备可从app获得bind_token。
     *        2.设备将bind_token发送至云平台完成设备添加。
     * 
     * @param bind_token: 最大长度64字节
     * @return ez_err_t 
     */
    EZOS_API ez_err_t ez_iot_base_bind_near(ez_char_t *bind_token);

    /**
     * @brief 物理确权添加，通常用于有线、4G等设备，使用挑战-响应认证模型。
     *        1.APP拿到设备序列号，发起添加。
     *        2.服务下发挑战请求(通过ez_base_notice回调)。
     *        3.设备按键、网线等方法进行物理确权。
     *        4.调用此接口完成响应。
     * 
     * @param challenge_code 服务下发的挑战信息
     * @return ez_err_t 
     */
    EZOS_API ez_err_t ez_iot_base_bind_response(ez_int32_t challenge_code);

    /**
     * @brief 
     * 
     * @param pfunc 
     * @return ez_base_err_e 
     */
    EZOS_API ez_void_t ez_iot_base_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
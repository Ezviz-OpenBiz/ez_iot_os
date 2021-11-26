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
 *******************************************************************************/

#ifndef _EZ_IOT_LOG_H_
#define _EZ_IOT_LOG_H_

#include <ezos.h>

/* output log's level */
#define EZ_ELOG_LVL_ASSERT 0  ///< 致命错误，导致整个程序无法继续运行
#define EZ_ELOG_LVL_ERROR 1   ///< 某个业务出错，不影响其他业务
#define EZ_ELOG_LVL_WARN 2    ///< 打印业务过程中必要的关键信息，尽量简短（WARN<=会记入文件）
#define EZ_ELOG_LVL_INFO 3    ///< 较详细的信息（不允许刷屏）
#define EZ_ELOG_LVL_DEBUG 4   ///< 更为详细的信息，每行带有行号（不允许刷屏）
#define EZ_ELOG_LVL_VERBOSE 5 ///< 不限制打印，每行带有行号，默认不开启。（不允许刷屏）

#define TAG_CORE "T_CORE"
#define TAG_SHD "T_SHD"
#define TAG_AP "T_AP"
#define TAG_OTA "T_OTA"
#define TAG_APP "T_APP"
#define TAG_BASE "T_BASE"
#define TAG_TIME "T_TIME"
#define TAG_ATCMD "T_ATCMD"
#define TAG_BLE "T_BLE"
#define TAG_HUB "T_HUB"
#define TAG_TSL "T_TSL"
#define TAG_AT "T_AT"

#ifdef __cplusplus
extern "C"
{
#endif

    ez_int32_t ezlog_init(ez_void_t);
    ez_void_t ezlog_start(ez_void_t);
    ez_void_t ezlog_stop(ez_void_t);

    /**
     * set log filter's level\tag\keyword
     */
    ez_void_t ezlog_filter_lvl(ez_uint8_t level);
    ez_void_t ezlog_filter_tag(const ez_char_t *tag);
    ez_void_t ezlog_filter_kw(const ez_char_t *keyword);

    extern ez_void_t elog_output(ez_uint8_t level, const ez_char_t *tag, const ez_char_t *file, const ez_char_t *func,
                                 const ez_long_t line, const ez_char_t *format, ...);

    extern void elog_hexdump(const char *name, ez_uint8_t width, ez_uint8_t *buf, ez_uint16_t size);

    #define ezlog_a(tag, ...) elog_output(EZ_ELOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_e(tag, ...) elog_output(EZ_ELOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_w(tag, ...) elog_output(EZ_ELOG_LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_i(tag, ...) elog_output(EZ_ELOG_LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_d(tag, ...) elog_output(EZ_ELOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_v(tag, ...) elog_output(EZ_ELOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ezlog_hexdump(tag, width, buf, size) elog_hexdump(tag, width, buf, size)

#ifdef __cplusplus
}
#endif

#endif
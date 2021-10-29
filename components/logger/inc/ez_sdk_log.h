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
* Contributors:
 *    shenhongyin - initial API and implementation and/or initial documentation
 *******************************************************************************/
#ifndef _EZ_SDK_LOG_H_
#define _EZ_SDK_LOG_H_

#include "ezos_file.h"
#include "ezos_io.h"
#include "ezos_mem.h"
#include "ezos_network.h"
#include "ezos_thread.h"
#include "ezos_time.h"
#include <stdint.h>

#if (defined(_WIN32) || defined(_WIN64))
#  if defined(EZOS_API_EXPORTS)
#    define EZOS_API __declspec(dllexport)
#  else
#    define EZOS_API __declspec(dllimport)
#  endif
#  define EZOS_CALL __stdcall
#elif defined(__linux__)
#  define EZOS_API
#  define EZOS_CALL
#else
#  define EZOS_API
#  define EZOS_CALL
#endif


/* output log's level */

#define LVL_ERROR   1  ///< err info 
#define LVL_WARN    2  ///< warning info
#define LVL_INFO    3  ///< common info
#define LVL_DEBUG   4  ///< debug info
#define LVL_VERBOSE 5  ///< no limit

#define TAG_SDK      "T_SDK"
#define TAG_SHADOW   "T_SHADOW"
#define TAG_MOD      "T_MODEL"
#define TAG_OTA      "T_OTA"
#define TAG_APP      "T_APP"
#define TAG_BASE     "T_BASE"
#define TAG_MICRO    "T_MICRO"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * 
     *start run log
     */
    EZOS_API int32_t EZOS_CALL ez_sdk_log_start(void);
    /**
     * stop  run log 
     */
    EZOS_API void EZOS_CALL ez_sdk_log_stop(void);

    /**
     * set log  level
     */
    EZOS_API void EZOS_CALL ez_sdk_set_log_level(uint8_t level);

    /**
     * filter log  by tag
     */
    EZOS_API void EZOS_CALL ez_sdk_log_filter_tag(const char *tag);

    /**
    * set log text color status
    */

    EZOS_API void EZOS_CALL ez_sdk_set_text_color_enabled(uint8_t enabled);

    /**
     * dump the hex format data to log
     *
     * @param name name for hex object, it will show on log header
     * @param width hex number for every line, such as: 16, 32
     * @param buf hex buffer
     * @param size buffer size
     */
    EZOS_API void EZOS_CALL ez_sdk_log_hexdump(const char *tag, uint8_t width, uint8_t *buf, uint16_t size);
    /**
     * print log
     */
    EZOS_API void EZOS_CALL elog_output(uint8_t level, const char *tag, const char *file, const char *func,const long line, const char *format, ...);
 
    #define ez_log_e(tag, ...) elog_output(LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ez_log_w(tag, ...) elog_output(LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ez_log_i(tag, ...) elog_output(LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ez_log_d(tag, ...) elog_output(LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define ez_log_v(tag, ...) elog_output(LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
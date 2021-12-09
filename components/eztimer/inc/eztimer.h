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
 * chentengfei (chentengfei5@ezvizlife.com)
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-24     chentengfei    first version
 *******************************************************************************/
#ifndef __COMMON_TIMER_TIMER_H__
#define __COMMON_TIMER_TIMER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ezos_def.h"

    /**
     * @brief           creat timer. if timer thread was not created before, this interface will creat timer thread which default stack is 4 * 1024 Bytes.
     * 
     * @param name      timer name
     * @param time_out  time out, ms
     * @param reload    support reload or not
     * @param fun       callback of the timer
     * @return void*    return handle of new timer
     */
    void *eztimer_create(ez_char_t *name, ez_int32_t time_out, ez_bool_t reload, void (*fun)(void));

    /**
     * @brief           reset the timer which handle is specified
     * 
     * @param handle    handle of the timer need to be reset, which return by eztimer_creates
     * @return ez_int32_t   0 for succ, other for failed
     */
    ez_int32_t eztimer_reset(void *handle);

    /**
     * @brief           change the period of the timer
     *
     * @param handle    handle of the timer
     * @param new_time_out  new period of the timer to be set
     * @return ez_int32_t   0 for succ, other for failed
     */
    ez_int32_t eztimer_change_period(void *handle, ez_int32_t new_time_out);

    /**
     * @brief           delete timer, if all timer are deleted, this interface will destroy the timer thread. 
     *
     * @param handle    handler of the timer to be deleted
     * @return ez_int32_t   0 for succ, other for failed
     */
    ez_int32_t eztimer_delete(void *handle);

#ifdef __cplusplus
}
#endif

#endif

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
 * 2021-11-22     xurongjun    first version 
 *******************************************************************************/

#include "misc.h"

#define TIMESPEC_THOUSAND 1000
#define TIMESPEC_MILLION 1000000
#define TIMESPEC_BILLION 1000000000

void bin2hexstr(const unsigned char *src, int len, int upper, unsigned char *dst)
{
    int i;
    char fmt[8];

    if (upper)
    {
        ezos_strcpy(fmt, "%02X");
    }
    else
    {
        ezos_strcpy(fmt, "%02x");
    }

    for (i = 0; i < len; ++i)
    {
        ezos_sprintf((char *)dst + 2 * i, fmt, src[i]);
    }
}

ez_bool_t time_isexpired(ezos_timespec_t *assign_timer)
{
    ezos_timespec_t now, res;
    if (NULL == assign_timer)
    {
        return ez_true;
    }

    ezos_get_clock(&now);
    res.tv_sec = assign_timer->tv_sec - now.tv_sec;
    res.tv_nsec = assign_timer->tv_nsec - now.tv_nsec;
    if (res.tv_nsec < 0)
    {
        --res.tv_sec;
        res.tv_nsec += TIMESPEC_BILLION;
    }

    return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_nsec <= 0);
}

ez_void_t time_countdown(ezos_timespec_t *assign_timer, ezos_time_t timeout_ms)
{
    ezos_timespec_t now;
    ezos_timespec_t interval = {timeout_ms / TIMESPEC_THOUSAND, (timeout_ms % TIMESPEC_THOUSAND) * TIMESPEC_MILLION};

    if (NULL == assign_timer)
    {
        return;
    }

    ezos_get_clock(&now);
    assign_timer->tv_sec = interval.tv_sec + now.tv_sec;
    assign_timer->tv_nsec = interval.tv_nsec + now.tv_nsec;

    if (assign_timer->tv_nsec >= TIMESPEC_BILLION)
    {
        ++assign_timer->tv_sec;
        assign_timer->tv_nsec -= TIMESPEC_BILLION;
    }
}
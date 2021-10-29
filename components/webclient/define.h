#ifndef _EZ_HTTP_DEFINE_H
#define _EZ_HTTP_DEFINE_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
//#include <netdb.h>
//#include <unistd.h>
#include "ezos_file.h"
#include "ezos_io.h"
#include "ezos_mem.h"
#include "ezos_network.h"
#include "ezos_thread.h"
#include "ezos_time.h"

//#define _POSIX_C_SOURCE 200112L
#define RT_USING_SAL

#define RT_NULL 0
#define RT_FALSE false
#define rt_tick_from_millisecond(ms) (ms)

#define RT_ASSERT(p) \
    if (NULL == p)   \
        ;
#define LOG_E printf
#define LOG_D printf

#ifndef rt_bool_t
#define rt_bool_t bool
#endif

#ifndef rt_strlen
#define rt_strlen strlen
#endif

#ifndef rt_strncpy
#define rt_strncpy strncpy
#endif

#ifndef rt_strstr
#define rt_strstr strstr
#endif

#ifndef rt_malloc
#define rt_malloc ezos_malloc
#endif

#ifndef rt_calloc
#define rt_calloc ezos_calloc
#endif

#ifndef rt_realloc
#define rt_realloc ezos_realloc
#endif

#ifndef rt_memset
#define rt_memset memset
#endif

#ifndef rt_free
#define rt_free ezos_free
#endif

#ifndef rt_strdup
#define rt_strdup strdup
#endif

#ifndef rt_int32_t
#define rt_int32_t int
#endif

#ifndef rt_vsnprintf
#define rt_vsnprintf vsnprintf
#endif

#ifndef rt_snprintf
#define rt_snprintf ezos_snprintf
#endif

#ifndef rt_strcmp
#define rt_strcmp strcmp
#endif

#endif
/**
 * @file uuid.h
 * @brief   
 * @author chenguanlan@hikvision.com
 * @version 1
 * @date 2013-08-28
 */

#ifndef __UTIL_UUID_H__
#define __UTIL_UUID_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned long unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char unsigned8;
typedef unsigned char byte;
#define CLOCK_SEQ_LAST 0x3FFF
#define RAND_MASK CLOCK_SEQ_LAST

typedef struct _uuid_t
{
    unsigned32 time_low;
    unsigned16 time_mid;
    unsigned16 time_hi_and_version;
    unsigned8 clock_seq_hi_and_reserved;
    unsigned8 clock_seq_low;
    byte node[6];
} ieee_uuid_t;

void CreateUUID(ieee_uuid_t* uuid);
void CreateStringUUID(ieee_uuid_t u, char* uuid);

#ifdef __cplusplus
}
#endif

#endif

/**
 *  \file      
 *  \filename  ezdev_sdk_kernel_ex.h  
 *  \filepath  Y:\makedir\devsdk_v2.0.0\microkernel\src\ezdev_sdk_kernel_ex.h
 *  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *  \brief     ΢�ں˶����һЩ�ӿ�
 *  \author    panlong
 *  \date      2017/6/1
 */

/**
* @addtogroup micro_kernel
*
* @section ΢�ں˶���ӿ�
* �ṩһЩԭ����ְ���ϲ�����΢�ں˵Ĺ��� 
*
*  ΢�ں˻�ȡSTUN��������Ϣ�ӿ�
* (see \c ezdev_sdk_kernel_get_stun()).
* 
*/

#ifndef H_EZDEV_SDK_KERNEL_EX_H_
#define H_EZDEV_SDK_KERNEL_EX_H_

#include "ezdev_sdk_kernel_struct.h"
#include "ezdev_sdk_kernel_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 *  \brief		��ȡSTUN��������Ϣ�ӿ�
 *  \method		ezdev_sdk_kernel_get_stun
 *  \param[in] 	stun_info * ptr_stun
 *  \return 	EZDEV_SDK_KERNEL_API ezdev_sdk_kernel_error
 */
EZDEV_SDK_KERNEL_API ezdev_sdk_kernel_error ezdev_sdk_kernel_get_stun(stun_info* ptr_stun, EZDEV_SDK_BOOL bforce_refresh);

/** 
 *  \brief		��������ʱ��
 *  \method		ezdev_sdk_kernel_set_keepalive_interval
 *  \param[in] 	EZDEV_SDK_UINT16 internal   �����������ʱ��
 *  \param[in]  EZDEV_SDK_UINT16 timeout_s  �ȴ���������Ӧ�ĳ�ʱʱ�䣬0��ʾ���ȴ�
 *  \return 	������ȴ���Ӧ�������ͽ����з��سɹ�����֮�ȴ�ƽ̨��Ӧ�����룬�����ʱ�����صȴ����ʱ��
 */
EZDEV_SDK_KERNEL_API ezdev_sdk_kernel_error ezdev_sdk_kernel_set_keepalive_interval(EZDEV_SDK_UINT16 internal, EZDEV_SDK_UINT16 timeout_s);

#ifdef __cplusplus
}
#endif

#endif
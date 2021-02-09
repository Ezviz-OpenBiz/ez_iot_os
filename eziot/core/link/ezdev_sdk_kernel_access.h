/**
*  \file      
*  \filename  ezdev_sdk_kernel_access.h  
*  \filepath  E:\workdir\С��Ŀ\ezDevSDK_v2.0.0\microkernel\src\ezdev_sdk_kernel_access.h
*  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
*  \brief     �豸SDK �������� ������������������볡���� 
*  \author    panlong
*  \date      2017/3/3
*/

#ifndef H_EZDEV_SDK_KERNEL_ACCESS_H_
#define H_EZDEV_SDK_KERNEL_ACCESS_H_

#define EZDEV_SDK_KERNEL_ACCESS_INTERFACE  \
	extern mkernel_internal_error stop_yield(ezdev_sdk_kernel* sdk_kernel); \
	extern mkernel_internal_error access_server_yield(ezdev_sdk_kernel* sdk_kernel);\
    extern mkernel_internal_error stop_das_logout(ezdev_sdk_kernel* sdk_kernel); \
    extern mkernel_internal_error stop_recieve_send_msg(ezdev_sdk_kernel* sdk_kernel); \
    extern mkernel_internal_error send_offline_msg_to_platform(EZDEV_SDK_UINT32 seq);

#endif //H_EZDEV_SDK_KERNEL_ACCESS_H_
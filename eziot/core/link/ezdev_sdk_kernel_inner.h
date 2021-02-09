#ifndef H_EZDEV_SDK_KERNEL_INNER_H_
#define H_EZDEV_SDK_KERNEL_INNER_H_

#include "ezdev_sdk_kernel_struct.h"
#include "ezdev_sdk_kernel_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

	/** 
	 *  \brief		����SDK���汾��(ֻ������SDK���ã�������΢�ں˵Ļ�����Ҫ����) �ڵ���ezdev_sdk_kernel_start֮ǰ����
	 *  \method		ezdev_sdk_kernel_set_sdk_main_version
	 *  \param[in] 	char szMainVersion[ezdev_sdk_extend_name_len]
	 *  \return 	EZDEV_SDK_KERNEL_API ezdev_sdk_kernel_error
	 */
	EZDEV_SDK_KERNEL_API ezdev_sdk_kernel_error ezdev_sdk_kernel_set_sdk_main_version(char szMainVersion[version_max_len]);

#ifdef __cplusplus
}
#endif

#endif
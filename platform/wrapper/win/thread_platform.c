/**
 * \file		thread_platform.c
 *
 * \brief		�߳���ز����ӿ�Windowsʵ��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		xurongjun
 *
 * \date		2018/6/27
 */

#include "thread_platform_wrapper.h"
#include <process.h>
#include <windows.h>
#include "ezdev_sdk_kernel_struct.h"

/** 
 *  \brief		�̺߳���
 *  \method		sdk_thread_fun
 *  \param[in] 	arg	�߳�������
 *  \return 	�߳��˳���,Ĭ��Ϊ0
 */
unsigned int __stdcall sdk_thread_fun( void * arg)
{
	thread_handle* hd = (thread_handle*)arg;
	if (hd == NULL)
	{
		return 0;
	}
	hd->task_do(hd->thread_arg);
	return 0;
}

/** 
 *  \brief		�߳������󴴽�
 *  \method		sdk_platform_thread_mutex_create
 *  \return 	�ɹ������߳������� ʧ�ܷ���NULL
 */
ezdev_sdk_mutex sdk_platform_thread_mutex_create()
{
	sdk_mutex_platform* ptr_mutex_platform = NULL;
	ptr_mutex_platform = (sdk_mutex_platform*)malloc(sizeof(sdk_mutex_platform));
	if (ptr_mutex_platform == NULL)
	{
		return NULL;
	}
	InitializeCriticalSection(&ptr_mutex_platform->lock);
	return (ezdev_sdk_mutex)ptr_mutex_platform;
}

/** 
 *  \brief		�߳�����������
 *  \method		sdk_platform_thread_mutex_destroy
 *  \param[in] 	arg	�߳�������
 */
void sdk_platform_thread_mutex_destroy(ezdev_sdk_mutex ptr_mutex)
{
	sdk_mutex_platform* ptr_mutex_platform = (sdk_mutex_platform*)ptr_mutex;
	if (ptr_mutex_platform == NULL)
	{
		return;
	}
	DeleteCriticalSection(&ptr_mutex_platform->lock);

	free(ptr_mutex_platform);
	ptr_mutex_platform = NULL;
}

/** 
 *  \brief		�߳�����
 *  \method		sdk_platform_thread_mutex_lock
 *  \param[in] 	ptr_mutex �߳�������
 *  \return 	�ɹ�����0 ʧ�ܷ���-1
 */
int sdk_platform_thread_mutex_lock(ezdev_sdk_mutex ptr_mutex)
{
	sdk_mutex_platform* ptr_mutex_platform = (sdk_mutex_platform*)ptr_mutex;
	if (ptr_mutex_platform == NULL)
	{
		return -1;
	}
	EnterCriticalSection(&ptr_mutex_platform->lock);
	return 0;
}

/** 
 *  \brief		�߳̽���
 *  \method		sdk_platform_thread_mutex_unlock
 *  \param[in] 	ptr_mutex �߳�������
 *  \return 	�ɹ�����0 ʧ�ܷ���-1
 */
int sdk_platform_thread_mutex_unlock(ezdev_sdk_mutex ptr_mutex)
{
	sdk_mutex_platform* ptr_mutex_platform = (sdk_mutex_platform*)ptr_mutex;
	if (ptr_mutex_platform == NULL)
	{
		return -1;
	}
	LeaveCriticalSection(&ptr_mutex_platform->lock);
	return 0;
}

int sdk_thread_create(thread_handle* handle)
{
	unsigned int threadID = 0;
	if (handle == NULL)
	{
		return -1;
	}
	handle->thread_hd = (HANDLE)_beginthreadex( NULL, 0, sdk_thread_fun, handle, 0, &threadID);
	if(handle->thread_hd == NULL)
	{
		return -1;
	}
	return 0;
}

int sdk_thread_destroy(thread_handle* handle)
{
	if (handle == NULL)
	{
		return -1;
	}
	if(handle->thread_hd != NULL)
	{
		WaitForSingleObject(handle->thread_hd, INFINITE);
		CloseHandle(handle->thread_hd);
		handle->thread_hd = NULL;
	}
}

void sdk_thread_sleep(unsigned int time_ms)
{
	Sleep(time_ms);
}
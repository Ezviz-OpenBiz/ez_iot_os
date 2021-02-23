/**
 * \file		thread_interface.h
 *
 * \brief		�߳���ز����ӿڣ���ͬƽ̨��ͬ���Ŀ¼���ж�Ӧ��ʵ��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		xurongjun
 *
 * \date		2018/6/27
 */
#ifndef H_THREAD_INTERFACE_H_
#define H_THREAD_INTERFACE_H_

#include "thread_platform_wrapper.h"


/** 
 *  \brief		�����߳�
 *  \method		sdk_thread_create
 *  \param[in] 	handle �߳�������
 *  \return 	�ɹ�����0 ʧ�ܷ���-1
 */
int sdk_thread_create(thread_handle* handle);

/** 
 *  \brief		�߳�����
 *  \method		sdk_thread_destroy
 *  \param[in] 	handle �߳�������
 *  \return 	�ɹ�����0 ʧ�ܷ���-1
 */
int sdk_thread_destroy(thread_handle* handle);

/** 
 *  \brief		�߳�����
 *  \method		sdk_thread_sleep
 *  \param[in] 	time_ms ����ʱ�䣨���룩
 */
void sdk_thread_sleep(unsigned int time_ms);

//typedef struct thread_handle_platform thread_handle;

#endif
/**
 * \file		time_platform.c
 *
 * \brief		ʱ����ؽӿڵ�Windowsʵ��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		xurongjun
 *
 * \date		2018/6/27
 */

#include "time_platform_wrapper.h"
#include <time.h>
#include "ezdev_sdk_kernel_struct.h"

/** 
 *  \brief		����ʱ�����
 *  \method		Platform_TimerCreater
 *  \return 	�ɹ�����ʱ����� ʧ�ܷ���NULL
 */
ezdev_sdk_time Platform_TimerCreater()
{
	win_time* wintime = NULL;
	wintime = (win_time*)malloc(sizeof(win_time));
	if (wintime == NULL)
	{
		return NULL;
	}

	wintime->time_record = 0;
	return wintime;
}

/** 
 *  \brief		�ж��Ƿ����
 *  \note		���ڵ�ʱ��͹�ȥ��ʱ��sdktime��ȣ��Ƿ��Ѿ�������time_ms��������Ϊ����
 *  \method		Platform_TimeIsExpired_Bydiff
 *  \param[in] 	sdktime ֮ǰ��ʱ�����
 *  \param[in] 	time_ms ʱ���жϻ�׼
 *  \return 	���ڷ�1 δ���ڷ�0
 */
char Platform_TimeIsExpired_Bydiff(ezdev_sdk_time sdktime, EZDEV_SDK_UINT32 time_ms)
{
	win_time* wintime = (win_time*)sdktime;
	time_t now_t = 0;
	if (wintime == NULL)
	{
		return (char)1;
	}

	now_t = time(NULL);
	if ((now_t - wintime->time_record) > (time_ms/1000 + 1))
	{
		return (char)1;
	}
	else
	{
		return (char)0;
	}
}

/** 
 *  \brief		�ж��Ƿ����
 *  \note		�ж�sdktime�Ƿ��ѹ��ڣ���������Ļ�Ϊ����
 *  \method		Platform_TimerIsExpired
 *  \param[in] 	sdktime ʱ�����
 *  \return 	���ڷ�1 δ���ڷ�0
 */
char Platform_TimerIsExpired(ezdev_sdk_time sdktime)
{
	// 	struct timeval now, res;
	// 	gettimeofday(&now, NULL);
	// 	timersub(&timer->end_time, &now, &res);		
	// 	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);

	time_t now_t = 0;
	win_time* wintime = (win_time*)sdktime;
	if (wintime == NULL)
	{
		return (char)1;
	}

	now_t = time(NULL);

	if (now_t >= wintime->time_record)
	{
		return (char)1;
	}
	else
	{
		return (char)0;
	}
}

/** 
 *  \brief		��ȡδ�����ٺ�������ʱ��
 *  \method		Platform_TimerCountdownMS
 *  \param[in] 	sdktime ʱ�����
 *  \param[in] 	timeout δ�����ٺ���
 */
void Platform_TimerCountdownMS(ezdev_sdk_time sdktime, unsigned int timeout)
{
	// 	struct timeval now;
	// 	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	// 	gettimeofday(&now, NULL);
	// 	timeradd(&now, &interval, &timer->end_time);

	time_t now_t = 0;
	win_time* wintime = (win_time*)sdktime;
	if (wintime == NULL)
	{
		return (char)1;
	}

	now_t = time(NULL);

	wintime->time_record = time(NULL);

	wintime->time_record += (timeout/1000 + 1);
}

/** 
 *  \brief		��ȡδ������������ʱ��
 *  \method		Platform_TimerCountdown
 *  \param[in] 	sdktime ʱ�����
 *  \param[in] 	timeout δ��������
 */
void Platform_TimerCountdown(ezdev_sdk_time sdktime, unsigned int timeout)
{
	// 	struct timeval now;
	// 	struct timeval interval = {timeout, 0};
	// 	gettimeofday(&now, NULL);
	// 	timeradd(&now, &interval, &timer->end_time);

	time_t now_t = 0;
	win_time* wintime = (win_time*)sdktime;
	if (wintime == NULL)
	{
		return (char)1;
	}

	now_t = time(NULL);

	wintime->time_record = time(NULL);
	wintime->time_record += timeout;
}

/** 
 *  \brief		��ȡʣ��ʱ�� ��������ʱ
 *  \method		Platform_TimerCountdown
 *  \param[in] 	sdktime ����ʱ����
 *  \return 	����ʣ��ʱ�� ����ѹ��ڷ���0
 */
EZDEV_SDK_UINT32 Platform_TimerLeftMS(ezdev_sdk_time sdktime)
{
	// 	struct timeval now, res;
	// 	gettimeofday(&now, NULL);
	// 	timersub(&timer->end_time, &now, &res);
	// 	//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	// 	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;

	time_t now_t = 0;
	win_time* wintime = (win_time*)sdktime;
	if (wintime == NULL)
	{
		return (char)1;
	}
	now_t = time(NULL);

	if (now_t > wintime->time_record)
	{
		return 0;
	}
	else
	{
		return (wintime->time_record-now_t)* 1000;
	}
}

/** 
 *  \brief		����ʱ�����
 *  \method		Platform_TimeDestroy
 *  \param[in] 	sdktime ʱ�����
 */
void Platform_TimeDestroy(ezdev_sdk_time sdktime)
{
	time_t now_t = 0;
	win_time* wintime = (win_time*)sdktime;
	if (wintime == NULL)
	{
		return (char)1;
	}
	
	free(wintime);
	wintime = NULL;
}

/**
 * \file		ezDevSDK_config.h
 *
 * \brief		�����ļ��Ĵ�ȡ
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		xurongjun
 *
 * \date		2018/6/27
 */

#ifndef H_EZDEVSDK_CONFIG_H_
#define H_EZDEVSDK_CONFIG_H_

/** 
 *  \brief		devinfo�����ļ��Ķ�ȡ
 *  \method		get_devinfo_fromconfig
 *  \param[in]	path				·��
 *  \param[out]	devinfo_context		���ݵ�ַ
 *  \param[in]	devinfo_context_len	���ݳ���
 *  \returns    �ɹ�����0,ʧ�ܷ���-1
 */
int get_devinfo_fromconfig(const char* path, char* devinfo_context, int devinfo_context_len);

/** 
 *  \brief		���ݱ������ļ���Ŀǰ����devid��masterkey
 *  \method		set_file_value
 *  \param[in]	path				·��
 *  \param[in]	devinfo_context		���ݵ�ַ
 *  \param[in]	devinfo_context_len	���ݳ���
 *  \returns    �ɹ�����0,ʧ�ܷ���-1
 */
int set_file_value(const char* path, unsigned char* keyvalue, int keyvalue_size);

/** 
 *  \brief		���ļ���ȡ��Ŀǰ����devid��masterkey����ȡ����С��64�ֽ�
 *  \method		get_file_value
 *  \param[in]	path				·��
 *  \param[out]	devinfo_context		���ݵ�ַ
 *  \param[in]	devinfo_context_len	���ݳ���
 *  \returns    �ɹ�����0,ʧ�ܷ���-1
 */
int get_file_value(const char* path, unsigned char* keyvalue, int keyvalue_maxsize);

#endif
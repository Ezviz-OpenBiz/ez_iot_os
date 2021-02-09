/**
 * \file		ezdev_sdk_kernel_error.h
 *
 * \brief		�����豸SDK΢�ں˵Ĵ�����Ϣ
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		panlong
 *
 * \date		2017/3/2
 */

/**
* @addtogroup micro_kernel 
*
* @section ΢�ں˴�������Ϣ
*
*  \sa ezdev_sdk_kernel_error
* 
*/

#ifndef H_EZDEV_SDK_KERNEL_ERROR_H_
#define H_EZDEV_SDK_KERNEL_ERROR_H_

#if defined (_WIN32) || defined(_WIN64)

#ifdef MICRO_KERNEL_EXPORTS
#define EZDEV_SDK_KERNEL_API	__declspec(dllexport)
#else
#define EZDEV_SDK_KERNEL_API __declspec(dllimport)
#endif

#define EZDEV_SDK_KERNEL_CALLBACK __stdcall
#else
#define EZDEV_SDK_KERNEL_API 

#define EZDEV_SDK_KERNEL_CALLBACK 
#endif

#define BASE_ERROR		0xE0000000
#define NET_ERROR		0x00000100
#define LBS_ERROR		0x00010000
#define SECRETKEY_ERROR	0x00010100
#define DAS_ERROR		0x00020000

typedef enum 
{
	ezdev_sdk_kernel_succ 						= 0,								///< �ɹ�
	ezdev_sdk_kernel_internal					= BASE_ERROR+1,						///< �ڲ�����
	ezdev_sdk_kernel_invald_call				= BASE_ERROR+2,						///< ������������
	ezdev_sdk_kernel_params_invalid				= BASE_ERROR+3,						///< �����Ƿ�
	ezdev_sdk_kernel_buffer_too_small			= BASE_ERROR+4,						///< ��������С����
	ezdev_sdk_kernel_data_len_range				= BASE_ERROR+5,						///< ���ݴ�С������Χ
	ezdev_sdk_kernel_memory						= BASE_ERROR+6,						///< �ڴ��쳣
	ezdev_sdk_kernel_json_invalid				= BASE_ERROR+7,						///< �Ƿ���json����
	ezdev_sdk_kernel_json_format				= BASE_ERROR+8,						///< json��������
	ezdev_sdk_kernel_extend_no_find				= BASE_ERROR+9,						///< ����·���Ҳ�����Ӧ��ע������
	ezdev_sdk_kernel_extend_full				= BASE_ERROR+10,					///< ��չע������
	ezdev_sdk_kernel_extend_existed				= BASE_ERROR+11,					///< ��չ�Ѿ�ע��
	ezdev_sdk_kernel_queue_full					= BASE_ERROR+12,					///< ��Ϣ��������
	ezdev_sdk_kernel_value_load					= BASE_ERROR+13,					///< ��ȡ�豸����ʧ��
	ezdev_sdk_kernel_value_save					= BASE_ERROR+14,					///< �����������豸ʧ��
    ezdev_sdk_kernel_msg_stop_distribute	    = BASE_ERROR+15,					///< �豸����ֹͣ,�ϲ���Ϣֹͣ�·�

	ezdev_sdk_kernel_net_create					= (BASE_ERROR+NET_ERROR)+1,			///< ����socketʧ��
	ezdev_sdk_kernel_net_connect				= (BASE_ERROR+NET_ERROR)+2,			///< ��������ʧ��
	ezdev_sdk_kernel_net_disconnected			= (BASE_ERROR+NET_ERROR)+3,			///< �������ӶϿ�
	ezdev_sdk_kernel_net_transmit				= (BASE_ERROR+NET_ERROR)+4,			///< ���ݴ���ʧ��
	ezdev_sdk_kernel_net_dns					= (BASE_ERROR+NET_ERROR)+5,			///< ��������ʧ��

	ezdev_sdk_kernel_lbs_authcode_mismatch		= (BASE_ERROR+LBS_ERROR)+1,			///< ��֤�벻һ��
	ezdev_sdk_kernel_lbs_invalid_call			= (BASE_ERROR+LBS_ERROR)+2,			///< �ͷ������Ľ�������������
	ezdev_sdk_kernel_lbs_invalid_data			= (BASE_ERROR+LBS_ERROR)+3,			///< �������յ���������
	ezdev_sdk_kernel_lbs_devid_mismatch			= (BASE_ERROR+LBS_ERROR)+4,			///< devid��һ��
	ezdev_sdk_kernel_lbs_masterkey_mismatch		= (BASE_ERROR+LBS_ERROR)+5,			///< masterkey��һ��
	ezdev_sdk_kernel_lbs_sessionkey_mismatch	= (BASE_ERROR+LBS_ERROR)+6,			///< sessionkey��һ��
	ezdev_sdk_kernel_lbs_invalid_dev			= (BASE_ERROR+LBS_ERROR)+7,			///< ��Ч�豸
	ezdev_sdk_kernel_lbs_server_crypto			= (BASE_ERROR+LBS_ERROR)+8,			///< �������ӽ���ʧ��
	ezdev_sdk_kernel_lbs_get_data				= (BASE_ERROR+LBS_ERROR)+9,			///< ��ȡdas����stun��Ϣʧ��
	ezdev_sdk_kernel_lbs_server_exception		= (BASE_ERROR+LBS_ERROR)+10,		///< �������ڲ��쳣
	ezdev_sdk_kernel_lbs_check_sessionkey_fail  = (BASE_ERROR+LBS_ERROR)+11,		///< sessionkey��֤ʧ��
	ezdev_sdk_kernel_lbs_sign_check_fail        = (BASE_ERROR+LBS_ERROR)+12,		///< auth_iУ��signʧ�ܡ�����device idУ��signʧ��(������֤�����)

	ezdev_sdk_kernel_secretkey_decrypt_fail 	= (BASE_ERROR+SECRETKEY_ERROR)+0,	///< ����secretkey����ƽ̨����ʧ��
	ezdev_sdk_kernel_secretkey_overflow_windows = (BASE_ERROR+SECRETKEY_ERROR)+1,	///< ����secretkey�����ڴ�����
	ezdev_sdk_kernel_secretkey_no_user			= (BASE_ERROR+SECRETKEY_ERROR)+2,	///< �豸δ���û�
	ezdev_sdk_kernel_secretkey_sn_not_exist		= (BASE_ERROR+SECRETKEY_ERROR)+3,	///< �豸������
	ezdev_sdk_kernel_secretkey_again			= (BASE_ERROR+SECRETKEY_ERROR)+4,	///< �豸�ظ�����

 	ezdev_sdk_kernel_das_nosupport_protocol_ver	= (BASE_ERROR+DAS_ERROR)+1,			///< ��֧�ֵ�Э��汾
 	ezdev_sdk_kernel_das_client_id_invalid		= (BASE_ERROR+DAS_ERROR)+2,			///< ���ϸ�Ŀͻ��˱�ʶ��
 	ezdev_sdk_kernel_das_server_unusable		= (BASE_ERROR+DAS_ERROR)+3,			///< ����˲�����(�������ڲ��쳣)
 	ezdev_sdk_kernel_das_invalid_username		= (BASE_ERROR+DAS_ERROR)+4,			///< ��Ч���û��������루�ֽ׶��ݲ�ʹ�ã�
 	ezdev_sdk_kernel_das_unauthorized			= (BASE_ERROR+DAS_ERROR)+5,			///< δ��Ȩ���ֽ׶��ݲ�ʹ�ã�
	ezdev_sdk_kernel_das_session_invaild		= (BASE_ERROR+DAS_ERROR)+6,			///< ����ỰʧЧ
	ezdev_sdk_kernel_das_force_offline			= (BASE_ERROR+DAS_ERROR)+7,			///< �豸��ǿ������
 	ezdev_sdk_kernel_das_force_dev_risk			= (BASE_ERROR+DAS_ERROR)+8,			///< �豸����أ���������
	ezdev_sdk_kernel_das_force_domain_risk		= (BASE_ERROR+DAS_ERROR)+9,			///< ���򱻷��
	ezdev_sdk_kernel_das_force_cmd_risk			= (BASE_ERROR+DAS_ERROR)+10,		///< ָ����
}ezdev_sdk_kernel_error;

#endif


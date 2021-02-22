/**
 * \file		ezDevSDK_boot.h
 *
 * \brief		SDKBootģ�飬���ϲ�Ӧ���ṩ��ʼ��������ʼ����������ֹͣ΢�ں˵ȹ��ܽӿڣ��н�ʱ�䡢�̡߳�����Ⱥ�ƽ̨�����Դ�Ĺ���ʵ��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		panlong
 *
 * \date		2017/3/10
 */

#ifndef H_EZDEVSDK_BOOT_H_ 
#define H_EZDEVSDK_BOOT_H_

#include "base_typedef.h"

#if defined (_WIN32) || defined(_WIN64)

#ifdef EZDEVSDK_BOOT_EXPORTS
#define EZDEVSDK_BOOT_API	__declspec(dllexport)
#else
#define EZDEVSDK_BOOT_API __declspec(dllimport)
#endif

#define EZDEVSDK_BOOT_CALLBACK __stdcall
#else
#define EZDEVSDK_BOOT_API 

#define EZDEVSDK_BOOT_CALLBACK 
#endif

#define ezdev_sdk_ip_max_len			64		///<	ip�����
#define ezdev_sdk_sessionkey_len		16		///<	�豸�Ự��Կ���� ,Ĭ����16���ֽ�
#define ezdev_sdk_name_len				64		///<	�豸SDK һЩ�����ĳ���

/*! \addtogroup Boot_Module SDKBootģ��
 * SDKBootģ�飬���ϲ�Ӧ���ṩ��ʼ��������ʼ����������ֹͣ΢�ں˵ȹ��ܽӿڣ��н�ʱ�䡢�̡߳�����Ⱥ�ƽ̨�����Դ�Ĺ���ʵ��
 *  \{
 */


/**
 * \brief App֪ͨ��Ϣ
 */
typedef enum
{
	ezDevSDK_App_Event_Online,				          ///<	�豸����	context == sdk_sessionkey_context
	ezDevSDK_App_Event_Break,				          ///<	�豸����	context == sdk_offline_context
	ezDevSDK_App_Event_Switchover,			          ///<	ƽ̨�л�	context == sdk_switchover_context
	ezDevSDK_App_Event_Invaild_authcode,	          ///<	��֤�벻�Ϲ���û�а��û��������ϲ�APP����	event_context == null
	ezDevSDK_App_Event_fast_reg_online,		          ///<	event_context == sdk_sessionkey_context	�豸��������
	ezDevSDK_App_Event_Runtime_err,			          ///<	evnet_context == sdk_runtime_err_context �豸sdk����ʱ������Ϣ
	ezDevSDK_App_Event_Reconnect_success,             ///<  evnet_context == NULL �����ɹ��¼��ص�
	ezDevSDK_App_Event_heartbeat_interval_changed     ///<  evnet_context == int  �����ı��¼��ص�
}ezDevSDK_App_Event;

/**
 * \brief ��־����
 */
typedef enum
{
	ezDevSDK_log_error,				///<	����
	ezDevSDK_log_warn,				///<	����
	ezDevSDK_log_info,				///<	��Ϣ
	ezDevSDK_log_debug,				///<	����
	ezDevSDK_log_trace				///<	�켣
}ezDevSDK_App_LogLevel;

/**
 * \brief ��ȡ��������
 */
typedef enum
{
	ezDevSDK_keyvalue_devid,				///<	�豸Ψһ��ʶ  �״��豸���ߺ����� һ��Ҫд��flash
	ezDevSDK_keyvalue_masterkey,			///<	�豸masterkey �״��豸���ߺ����� ����д��flash
	ezDevSDK_keyvalue_count					///<	��ö������
}ezDevSDK_App_keyvalue_type;

/**
 * \brief ��ȡ��������
 */
typedef enum
{
	ezDevSDK_curingdata_secretkey,			///<	��֤�벻�Ϲ��豸���������secretkey��һ��Ҫ�̻�
	ezDevSDK_curingdata_count				///<	��ö������
}ezDevSDK_App_curingdata_type;

/** 
 *  \brief		�����¼�֪ͨ����
 *  \method		ezDevSDK_Event_Notice
 *  \param[in] 	event_id				��Ϣ����
 *  \param[in] 	context					��Ϣ�����������
 */
typedef void (*ezDevSDK_Event_Notice)(ezDevSDK_App_Event event_id, void * context);

/** 
 *  \brief		������־֪ͨ����
 *  \method		ezDevSDK_key_value_load
 *  \param[in] 	level					��־�ȼ�
 *  \param[in] 	sdk_error				SDK������
 *  \param[in] 	othercode				�����Ĵ�����
 *  \param[in] 	buf						��Ϣ��ŵ�ַ
 */
typedef void (*ezDevSDK_Log_Notice)(ezDevSDK_App_LogLevel level, EZDEV_SDK_INT32 sdk_error, EZDEV_SDK_INT32 othercode, const char *buf);

/** 
 *  \brief		������عؼ���Ϣ�ĺ���
 *  \method		ezDevSDK_key_value_load
 *  \param[in] 	valuetype				��������
 *  \param[in] 	keyvalue				���ݴ�ŵ�ַ
 *  \param[in] 	keyvalue_maxsize		���ݴ�����ֵ
 */
typedef void (*ezDevSDK_key_value_load)(ezDevSDK_App_keyvalue_type valuetype, unsigned char* keyvalue, EZDEV_SDK_UINT32 keyvalue_maxsize);

/** 
 *  \brief		����洢�ؼ���Ϣ�ĺ���
 *  \method		ezDevSDK_key_value_save
 *  \param[in] 	valuetype				��������
 *  \param[in] 	keyvalue				�������ݵ�ַ
 *  \param[in] 	keyvalue_size			�������ݳ���
 *  \return 	�ɹ�����0 ʧ�ܷ��ط�0
 */
typedef EZDEV_SDK_INT32 (*ezDevSDK_key_value_save)(ezDevSDK_App_keyvalue_type valuetype, unsigned char* keyvalue, EZDEV_SDK_UINT32 keyvalue_size);

/** 
 *  \brief			������عؼ���Ϣ�ĺ���
 *  \method			ezDevSDK_curing_data_load
 *  \param[in] 		valuetype				��������
 *  \param[in] 		keyvalue				���ݴ�ŵ�ַ
 *  \param[in\out] 	*keyvalue_maxsize		���ݻ��������ֵ,�������ݴ�С
 *  \return 		�ɹ�����0��ʧ�ܷ��ط�0
 */
typedef EZDEV_SDK_INT32 (*ezDevSDK_curing_data_load)(ezDevSDK_App_curingdata_type valuetype, unsigned char* keyvalue, EZDEV_SDK_UINT32 *keyvalue_maxsize);

/** 
 *  \brief		����洢�ؼ���Ϣ�ĺ���
 *  \method		ezDevSDK_curing_data_save
 *  \param[in]	valuetype				��������
 *  \param[in]	keyvalue				�������ݵ�ַ
 *  \param[in] 	keyvalue_size			�������ݳ���
 *  \return 	�ɹ�����0��ʧ�ܷ��ط�0
 */
typedef EZDEV_SDK_INT32 (*ezDevSDK_curing_data_save)(ezDevSDK_App_curingdata_type valuetype, unsigned char* keyvalue, EZDEV_SDK_UINT32 keyvalue_size);

/**
 * �͹����豸��������.
 */
typedef struct
{
	EZDEV_SDK_INT8 bLightreg;							///< ָ���Ƿ��������,0(��),1��wifi����������2��RF��������
	EZDEV_SDK_UINT16 das_port;							///< das�˿�
	EZDEV_SDK_UINT16 das_udp_port;						///< das udp�˿�
	int das_socket;										///< �ϴ����߱��޵�socket,���Բ�ָ��
	char das_address[ezdev_sdk_ip_max_len];				///< das IP��ַ
	char das_domain[ezdev_sdk_ip_max_len];				///< das ����
	char das_serverid[ezdev_sdk_name_len];				///< das serverid
	unsigned char session_key[ezdev_sdk_sessionkey_len];///< das session key
}ezDevSDK_das_info;

/**
 * ��ʼ��΢�ں˵Ľṹ��.
 * \note
 * - ���bUserΪ!0����Ҫʵ�ִ�ȡ�ص��������ڳ�ʼ��ʱ����
 * - ����΢�ں�ǰdevinfo���ļ�Ӧ��������Ч
 * - ����΢�ں�ǰdev_id��dev_masterkey����ֻ�ṩ·�������豸���ߺ�õ����ݻ���ݴ�·��д�ļ�
 */
typedef struct
{
	EZDEV_SDK_UINT8 bUser;								///< �Ƿ����û��ص� 0�ر� !0���� ���Ϊ0 dev_id&masterkeyͨ���ļ���ʽ��ȡ ����ͨ���ص��ķ�ʽ��ȡ
	ezDevSDK_key_value_load	keyValueLoadFun;			///< ����Ϣ�ĺ���
	ezDevSDK_key_value_save keyValueSaveFun;			///< д��Ϣ�ĺ���
	ezDevSDK_curing_data_load	curingDataLoadFun;		///< �̻����ݵĶ�����,����ʵ��
	ezDevSDK_curing_data_save 	curingDataSaveFun;		///< �̻����ݵ�д����,����ʵ��
	char devinfo_path[128];								///< devinfo�ļ�·��
	char dev_id[128];									///< dev_id�ļ�·��
	char dev_masterkey[128];							///< masterkey�ļ�·��
	ezDevSDK_das_info* reg_das_info;					///< �͹����豸��������,��Ҫ�ṩdas��Ϣ,�������ҪĬ��ΪNULL
}ezDevSDK_config;

typedef struct
{
	ezDevSDK_Event_Notice event_notice;			///<	�¼�֪ͨ�ص�
	ezDevSDK_Log_Notice log_notice;				///<	��־�ص�
}ezDevSDK_notice;

typedef struct
{
	ezDevSDK_notice	notice;						///<	��Ϣ֪ͨ���
	ezDevSDK_config config;						///<	�������
}ezDevSDK_all_config;

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 *  \brief		Bootģ���ʼ���ӿ�
 *  \method		ezDevSDK_Init
 *  \param[in] 	server_name			ƽ̨��������
 *  \param[in] 	server_port			ƽ̨����˿�
 *  \param[in] 	all_config			������Ϣ
 *  \param[in]  reg_mode			ע��ģʽ
 *  \details
 *	1---�����豸(ƽ̨Ĭ��ֵ)
 *	2---wifi�йܵ͹����豸(������,��ʾ����豸��ǰ�й�״̬)
 *	3---RF�йܵ͹����豸(��������, ��ʾ����豸��ǰ�й�״̬)
 *	4---RF����(��������, ��ʾ֧��RF�й�,��Base�豸�ϱ�)
 *  \return 	ezdev_sdk_kernel_succ��ezdev_sdk_kernel_params_invalid��ezdev_sdk_kernel_json_invalid��ezdev_sdk_kernel_json_format�� \n
 *				ezdev_sdk_kernel_value_load��ezdev_sdk_kernel_invald_call��ezdev_sdk_kernel_memory��ezdev_sdk_kernel_internal
 *	\see		������ �� ezdev_sdk_kernel_error
 */
EZDEVSDK_BOOT_API  EZDEV_SDK_INT32 ezDevSDK_Init(const char* server_name, EZDEV_SDK_UINT32 server_port, ezDevSDK_all_config* all_config, EZDEV_SDK_UINT32 reg_mode);

/** 
 *  \brief		Bootģ�鷴��ʼ���ӿ�
 *  \method		ezDevSDK_Fini
 *  \return 	�ɹ�����0 ʧ�����������
 *  \return 	ezdev_sdk_kernel_succ��ezdev_sdk_kernel_invald_call
 *	\see		������ �� ezdev_sdk_kernel_error
 */
EZDEVSDK_BOOT_API  EZDEV_SDK_INT32 ezDevSDK_Fini();

/** 
 *  \brief		Bootģ�������ӿڣ���������SDK��������ӿں�SDK���빤��ģʽ��
 *  \method		ezDevSDK_Start
 *  \return 	�ɹ�����0 ʧ�����������
 *  \return 	ezdev_sdk_kernel_succ��ezdev_sdk_kernel_invald_call
 *	\see		������ �� ezdev_sdk_kernel_error
 */
EZDEVSDK_BOOT_API  EZDEV_SDK_INT32 ezDevSDK_Start();

/** 
 *  \brief		Bootģ��ֹͣ�ӿ�
 *  \method		ezDevSDK_Stop
 *  \return 	�ɹ�����0 ʧ�����������
 *  \return 	ezdev_sdk_kernel_succ��ezdev_sdk_kernel_invald_call
 *	\see		������ �� ezdev_sdk_kernel_error
 */
EZDEVSDK_BOOT_API  EZDEV_SDK_INT32 ezDevSDK_Stop();

#ifdef __cplusplus
}
#endif
/*! \} */

#endif //H_EZDEVSDK_BOOT_H_
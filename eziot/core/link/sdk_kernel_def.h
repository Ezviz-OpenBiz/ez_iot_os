/**
 *  \file      
 *  \filename  sdk_kernel_def.h  
 *  \filepath  e:\workdir\С��Ŀ\ezDevSDK_v2.0.0\microkernel\src\sdk_kernel_def.h
 *  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *  \brief     �豸SDK �ڲ� �ṹ�塢ö���Լ��궨�� 
 *  \author    panlong
 *  \date      2017/3/4
 */
#ifndef H_SDK_KERNEL_DEF_H_
#define H_SDK_KERNEL_DEF_H_

#include "base_typedef.h"
#include "ezdev_sdk_kernel_struct.h"

#define ezdev_sdk_recv_topic_len									128		   ///<	�豸SDK һЩ�����ĳ���
#define ezdev_sdk_type_len											16		   ///<	�豸SDK ���ͳ���
#define ezdev_sdk_name_len											64		   ///<	�豸SDK һЩ�����ĳ���
#define ezdev_sdk_sharekey_len										32		   ///<	�豸share key ����
#define ezdev_sdk_devserial_maxlen									72		   ///< �豸���к�֧����󳤶�
#define ezdev_sdk_total_len											128		   ///<	������ʱ�����Ĵ��
#define ezdev_sdk_identificationcode_max_len						256		   ///<	�豸�̼�ʶ������󳤶�
#define ezdev_sdk_md5_len											32		   ///<	md5����
#define ezdev_sdk_sha256_len                                        32         ///<	sha256���ݳ���
#define ezdev_sdk_sha256_hex_len                                    64         ///<	sha256����16���Ƴ���
#define ezdev_sdk_sha256_offset                                     10         ///<	sha256����ƫ��ֵ
#define ezdev_sdk_productkey_len									32		   ///<	productkey��ĳ���
#define ezdev_sdk_json_default_size									1024	   ///<	bscJSON_PrintBuffered ����ʱ����Ĭ�ϴ�С�����ٶ��malloc/free����
#define ezdev_sdk_domain_id                                         1100       ///< �豸��������ʱ���ڲ�����������Ϣʹ�õ�����id
#define ezdev_sdk_offline_cmd_id                                    0X00002807 ///< �豸��������ʱ���͵�ָ��id
#define ezdev_sdk_cmd_version                                       "v1.0.0"   ///< ָ��汾
#define version_max_len					                            32		   ///<	�汾����
#define QOS_T1                                                      1          ///< ����Qos1

#define ezdev_sdk_pbkdf2_hmac_times                                 3          ///<	

#define SendISAPIReq                                                0x00004D01 ///< isapiЭ�飬����Э����˷��ͱ��Ĵ�С
#define SendISAPIRsp                                                0x00004D02 ///<
#define SendISAPIEventReq                                           0x00004D03 ///<
#define SendISAPIEventRsp                                           0x00004D03 ///<
#define ezdev_sdk_com_msg_max_buf                                   1024*16    ///< ��isapi��Ϣ������������͵�size
#define ezdev_sdk_tcp_header_len                                    79         ///< tcpͷ�ͻ��Ѱ�ͷ����


#define ezdev_sdk_model_type_len											16		   ///<	�豸SDK ���ͳ���

#ifdef RAM_LIMIT
//��Ҫ�Ǹ�realtekʹ��
#define ezdev_sdk_send_buf_max			1024*2
#define ezdev_sdk_recv_buf_max			1024*2

#define lbs_send_buf_max			1024*2
#define lbs_recv_buf_max			1024*2

#define	ezdev_sdk_extend_count  8				///<	֧�ֵ���չģ������

/**
* \brief   SDK һ������֧�ֵķ��ָ�� �����
*/
#define ezdev_sdk_risk_control_cmd_max		8
#define	ezdev_sdk_queue_max					32
#else  //RAM_LIMIT
/**
* \brief   DAS MQTT �Ựʹ�õĻ���
*/


#if RSETBUFFER          //ISAPI�ӿڷ��͵�buffer��С����Ϊ256k

#define ezdev_sdk_send_buf_max			1024*256
#define ezdev_sdk_recv_buf_max			1024*256

#else

#define ezdev_sdk_send_buf_max			1024*16
#define ezdev_sdk_recv_buf_max			1024*16

#endif

#define lbs_send_buf_max			1024*16
#define lbs_recv_buf_max			1024*16

#define ezdev_sdk_auth_group_size   64                      //֧�ֵ���֤Э���������������
#define lbs_var_head_buf_max ezdev_sdk_auth_group_size + 2  //�ɱ䱨��ͷ��󳤶ȣ�2���ֽڷֱ��ʾ��ǰЭ�����ͺ�Э���鵱ǰ����

#define	ezdev_sdk_extend_count	 32				///<	֧�ֵ���չģ������

/**
* \brief   SDK һ������֧�ֵķ��ָ�� �����
*/
#define ezdev_sdk_risk_control_cmd_max		64
#define	ezdev_sdk_queue_max						64


#endif //RAM_LIMIT

#define ezdev_sdk_das_default_keepaliveinterval			30		///<	DASĬ������ʱ��
#define ezdev_sdk_sharekey_salt "www.88075998.com"

#define ezdev_sdk_max_publish_count		2		///<	��෢���Ĵ���
#define ezdev_sdk_msg_type_req			1		///<	das��������:����
#define ezdev_sdk_msg_type_rsp			2		///<	das��������:��Ӧ

typedef enum
{
	sdk_no_risk_control=0,		///<	�����
	sdk_risk_control=1,			///<	���
}sdk_risk_control_flag;


// /**
// * \brief   �豸SDK��չģ��ID����
// */
// typedef enum
// {
// 	ezdev_sdk_extend_begin = 0,				///<	��չģ����ʼ
// 	ezdev_sdk_extend_count = 16				///<	֧�ֵ���չģ������
// }ezdev_sdk_extend_id;

/**
* \brief   ���SDK���״̬
*/
typedef enum
{
	sdk_entrance_normal,			///<	SDK�������
	sdk_entrance_switchover,			///<	SDK����л�
	sdk_entrance_authcode_invalid
}sdk_entrance_state;


/**
* \brief   ���SDK״̬
*/
typedef enum
{
	sdk_idle0 = 0,					///<	�մ���δ��ʼ��
	sdk_idle ,						///<	����		��ʼ��״̬
	sdk_start,						///<	sdk��������
	sdk_stop,						///<	sdk���ڹر�
	sdk_idle2,						///<	����		�رպ��״̬
}sdk_state;

/**
* \brief   ���SDK��cloud����״̬
*/
typedef enum
{
	sdk_cnt_unredirect = 0,				///<	δ�ض���״̬		��Ҫ��LBSȥ���ض���
	sdk_cnt_redirected = 1,				///<	�ض������			��Ҫ��DAS��ע��
	sdk_cnt_das_reged = 2,				///<	���ע��			��Ҫ���պͷ���ָ��
	sdk_cnt_das_break = 4,				///<	��DAS�����ж�״̬	��Ҫ����
	sdk_cnt_das_fast_reg = 5,           ///<	�豸��������
	sdk_cnt_das_fast_reg_v3 = 6,		///<    RF��������
}sdk_cloud_cnt_state;

typedef enum
{
	sdk_dev_auth_sap = 0,				///<	SAP��֤
	sdk_dev_auth_license = 1			///<	license��֤
}sdk_dev_auth_mode;


typedef enum
{
	sdk_v3_unreg = 0,				///<	V3Э��δʹ��
	sdk_v3_reged ,
}sdk_v3_reg_status;

typedef enum
{
    sdk_dev_auth_protocol_none = 0,     ///<	����֤
    sdk_dev_auth_protocol_ecdh = 1,     ///<	ECDH��֤
    //sdk_dev_auth_protocol_test = 4,     ///<	�����õ�
    sdk_dev_auth_protocol_end
}sdk_dev_auth_protocol_type;

/**
* \brief   ��LBS����Э��
*/
typedef struct
{
	unsigned char *head_buf; 
	EZDEV_SDK_UINT8 head_buf_Len;			///<	������5���ֽڴ�С����
	EZDEV_SDK_UINT8 head_buf_off;

    unsigned char *var_head_buf;
    EZDEV_SDK_UINT8 var_head_buf_Len;   //�ɱ䱨��ͷ
    EZDEV_SDK_UINT8 var_head_buf_off;

	unsigned char *payload_buf;
	EZDEV_SDK_UINT32 payload_buf_Len;
	EZDEV_SDK_UINT32 payload_buf_off;
}lbs_packet;

/**
* \brief   ��LBS����ͨ��Э����
*/
typedef struct
{
	unsigned char	pro_form_version;			///<	����Э����ʽ�汾��
	unsigned char	pro_type_low_version;		///<	����Э�����Ͱ汾��(��)
	unsigned char	pro_type_high_version;		///<	����Э�����Ͱ汾��(��)
}lbs_common;

typedef struct
{
	char server_name[ezdev_sdk_name_len];		///<	lbs ����
	char server_ip[ezdev_sdk_name_len];			///<	lbs Ip
	EZDEV_SDK_INT16 server_port;				///<	lbs port
}ezdev_server_info;

typedef struct
{
	EZDEV_SDK_UINT16	das_port;
	EZDEV_SDK_UINT16	das_udp_port;
	char das_address[ezdev_sdk_ip_max_len];
	char das_domain[ezdev_sdk_ip_max_len];
	char das_serverid[ezdev_sdk_name_len];
}das_info;

/**
 * \brief   �豸������Ϣ
 */
typedef struct
{
	EZDEV_SDK_UINT16	dev_access_mode;										///		�豸����ģʽ  0-��ͨ��2.0��   1-HUB��2.0��
	sdk_dev_auth_mode dev_auth_mode;											///<    ��֤ģʽ��0 SAP��֤   1 licence��֤
	EZDEV_SDK_UINT16 dev_status;												///<	�豸����״̬  1����������ģʽ  5������(��˯��)����ģʽ
	char dev_subserial[ezdev_sdk_devserial_maxlen];								///<	�豸�����к�(��Ӧlicence��֤��device_id)
	char dev_verification_code[ezdev_sdk_verify_code_maxlen];					///<	�豸��֤��(��Ӧlicence��֤��licence)
	char dev_serial[ezdev_sdk_devserial_maxlen];								///<	�豸�����к�
	char dev_firmwareversion[ezdev_sdk_name_len];								///<	�豸�̼��汾��
	char dev_type[ezdev_sdk_name_len];											///<	�豸�ͺ�
	char dev_typedisplay[ezdev_sdk_name_len];									///<	�豸��ʾ�ͺ�
	char dev_mac[ezdev_sdk_name_len];											///<	�豸���������ַ
	char dev_nickname[ezdev_sdk_name_len];										///<	�豸�ǳ�
	char dev_firmwareidentificationcode[ezdev_sdk_identificationcode_max_len];	///<	�豸�̼�ʶ����
	EZDEV_SDK_UINT32 dev_oeminfo;												///<	�豸��OEM��Ϣ
}dev_basic_info;

/**
* \brief   ������Ϣ
*/
typedef struct
{
	sdk_risk_control_flag		domain_risk;											///<	�����Ƿ񱻷��
	EZDEV_SDK_UINT32			cmd_risk_array[ezdev_sdk_risk_control_cmd_max];			///<	�����ڱ���ص�ָ��
	ezdev_sdk_kernel_extend		kernel_extend;											///<	SDKע�������������չ
}ezdev_sdk_kernel_domain_info;

/**
* \brief   ������Ϣ
*/
typedef struct
{
	ezdev_sdk_kernel_extend_v3		kernel_extend;											///<	SDKע�������������չ
}ezdev_sdk_kernel_domain_info_v3;


typedef struct
{
	EZDEV_SDK_UINT8		lbs_redirect_times;										///<	��¼lbs�����ض���Ĵ���
	EZDEV_SDK_UINT8		das_retry_times;										///<	��¼das���������Ĵ���
	EZDEV_SDK_BOOL		secretkey_applied;										///<	�Ƿ��Ѿ������secretkey
	EZDEV_SDK_UINT16	secretkey_interval;										///<	����secretkey��������Լ��(s)
	EZDEV_SDK_UINT32	secretkey_duration;										///<	����secretkey�����������ʱ��(s)
	sdk_v3_reg_status	v3_reg_status;										    ///<	V3Э���Ƿ�ʹ��

	sdk_entrance_state	entr_state;												///<	sdk���״̬
	sdk_state			my_state;												///<	sdk״̬
	sdk_cloud_cnt_state cnt_state;												///<	����״̬											
	ezdev_sdk_time		cnt_state_timer;										///<	������صĶ�ʱ��
	
	char dev_subserial[ezdev_sdk_devserial_maxlen];
	unsigned char master_key[ezdev_sdk_masterkey_len];
	unsigned char dev_id[ezdev_sdk_devid_len];
	unsigned char session_key[ezdev_sdk_sessionkey_len];

	EZDEV_SDK_UINT32			das_keepalive_interval;							///<	DAS����ʱ����
	dev_basic_info				dev_info;										///<	�豸������Ϣ

	ezdev_server_info			server_info;									///<	lbs������Ϣ
	das_info					redirect_das_info;								///<	lbs�ض��������das��Ϣ

	char subscribe_topic[ezdev_sdk_recv_topic_len];							    ///<	�豸��ƽ̨���ĵ�����

	char szMainVersion[version_max_len];										///<	SDK���汾��

	EZDEV_SDK_UINT8 reg_mode;													///<	�豸ע��ģʽ
	ezdev_sdk_kernel_platform_handle	platform_handle;						///<	lbs ������ʹ��
	sdk_risk_control_flag				access_risk;							///<	�����ر�ʶ

    EZDEV_SDK_UINT8     dev_cur_auth_type;
    EZDEV_SDK_UINT8     dev_def_auth_type;
    EZDEV_SDK_UINT8     dev_auth_type_count;
    EZDEV_SDK_UINT8     dev_last_auth_type;
    EZDEV_SDK_UINT8     dev_auth_type_group[ezdev_sdk_auth_group_size];
}ezdev_sdk_kernel;

typedef struct
{
	EZDEV_SDK_UINT8 random_1;
	EZDEV_SDK_UINT8 random_2;
	EZDEV_SDK_UINT8 random_3;
	EZDEV_SDK_UINT8 random_4;
	
	EZDEV_SDK_UINT16	dev_access_mode;
	sdk_dev_auth_mode dev_auth_mode;
	char dev_subserial[ezdev_sdk_devserial_maxlen];
	unsigned char master_key[ezdev_sdk_masterkey_len];
	unsigned char dev_id[ezdev_sdk_devid_len];

	unsigned char session_key[ezdev_sdk_sessionkey_len];
	unsigned char share_key[ezdev_sdk_sharekey_len];
	EZDEV_SDK_UINT16 share_key_len;

	lbs_packet global_out_packet;	///<*	lbs ���ͻ�����
	lbs_packet global_in_packet;	///<*	lbs ���ջ�����

	ezdev_sdk_net_work			lbs_net_work;
}lbs_affair;

/**
 * \brief   �豸������Ϣ����Ϣ�洢����
 */
typedef struct
{
	EZDEV_SDK_UINT16		max_send_count;			///<	��󷢲�������send��--
	ezdev_sdk_kernel_pubmsg		msg_conntext;		///<	��������Ϣ����
}ezdev_sdk_kernel_pubmsg_exchange;

/**
 * \brief   �豸������Ϣ����Ϣ�洢���� 3.0Э��
 */
typedef struct
{
	EZDEV_SDK_UINT16		max_send_count;			///<	��󷢲�������send��--
	ezdev_sdk_kernel_pubmsg_v3	msg_conntext_v3;		///<	��������Ϣ����
}ezdev_sdk_kernel_pubmsg_exchange_v3;

typedef enum
{
	extend_cb_start,				///<	ezdev_sdk_kernel_extend_start
	extend_cb_stop,					///<	ezdev_sdk_kernel_extend_stop
	extend_cb_event					///<	ezdev_sdk_kernel_extend_event
}extend_cb_type;

/**
* \brief   kernel�ڲ�ʹ�õ���չ�첽�ص���Ϣ
*/
typedef struct
{
	extend_cb_type	cb_type;
	ezdev_sdk_kernel_event	cb_event;
}ezdev_sdk_kernel_inner_cb_notic;

void ezdev_sdk_kernel_log (sdk_log_level level, int sdk_error, int othercode, \
						   const char *fmt, ...);

ezdev_sdk_kernel* get_ezdev_sdk_kernel();

#if defined (_WIN32) || defined(_WIN64)
#define ezdev_sdk_kernel_log_error(sdk_error, othercode, ...) ezdev_sdk_kernel_log(sdk_log_error, sdk_error, othercode, __VA_ARGS__)
#define ezdev_sdk_kernel_log_warn(sdk_error, othercode, ...) ezdev_sdk_kernel_log(sdk_log_warn, sdk_error, othercode, __VA_ARGS__)
#define ezdev_sdk_kernel_log_info(sdk_error, othercode, ...) ezdev_sdk_kernel_log(sdk_log_info, sdk_error, othercode, __VA_ARGS__)
#define ezdev_sdk_kernel_log_debug(sdk_error, othercode, ...) ezdev_sdk_kernel_log(sdk_log_debug, sdk_error, othercode, __VA_ARGS__)
#define ezdev_sdk_kernel_log_trace(sdk_error, othercode, ...) ezdev_sdk_kernel_log(sdk_log_trace, sdk_error, othercode, __VA_ARGS__)
#else
#define ezdev_sdk_kernel_log_error(sdk_error, othercode, args...) ezdev_sdk_kernel_log(sdk_log_error, sdk_error, othercode, ##args)
#define ezdev_sdk_kernel_log_warn(sdk_error, othercode, args...) ezdev_sdk_kernel_log(sdk_log_warn, sdk_error, othercode, ##args)
#define ezdev_sdk_kernel_log_info(sdk_error, othercode, args...) ezdev_sdk_kernel_log(sdk_log_info, sdk_error, othercode, ##args)
#define ezdev_sdk_kernel_log_debug(sdk_error, othercode, args...) ezdev_sdk_kernel_log(sdk_log_debug, sdk_error, othercode, ##args)
#define ezdev_sdk_kernel_log_trace(sdk_error, othercode, args...) ezdev_sdk_kernel_log(sdk_log_trace, sdk_error, othercode, ##args)
#endif

#define	global_ezdev_sdk_kernel get_ezdev_sdk_kernel()

#endif
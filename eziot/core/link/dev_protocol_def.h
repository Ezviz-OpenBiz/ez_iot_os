/**
*  \file      
*  \filename  dev_protocol_def.h  
*  \filepath  e:\workdir\С��Ŀ\ezDevSDK_v2.0.0\microkernel\src\dev_protocol_def.h
*  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
*  \brief     �豸Э��2.0�����ж���
*  \author    panlong
*  \date      2017/3/4
*/
#ifndef H_DEV_PROTOCOL_DEF_H_ 
#define H_DEV_PROTOCOL_DEF_H_


#define DEV_ACCESS_DOMAIN_VERSION									"V3.1.0"	///<	��ǰ��������İ汾��


#define DEV_PROTOCOL_AUTHENTICATION_I          						0x1			///<	�豸��ƽ̨��֤_I,ECDH��Կ��������
#define DEV_PROTOCOL_AUTHENTICATION_II          					0x2			///<	�豸��ƽ̨��֤_II,ECDH��Կ������Ӧ
#define DEV_PROTOCOL_AUTHENTICATION_III          					0x3			///<	����
#define DEV_PROTOCOL_REFRESHSESSIONKEY_REQ          		        0x4			///<	����SessionKey���� 
#define DEV_PROTOCOL_AUTHENTICATION_V          					    0x5			///<	����
#define DEV_PROTOCOL_REFRESHSESSIONKEY_RSP          		        0x6			///<	����SessionKey��Ӧ
#define DEV_PROTOCOL_REQUEST_DEVID       					        0x7			///<	�豸��ƽ̨�����豸ID
#define DEV_PROTOCOL_RESPONSE_DEVID           					    0x8			///<	ƽ̨���豸��Ӧ�豸ID
#define DEV_PROTOCOL_APPLY_DEVID_CFM        				        0x9			///<	�豸��ƽ̨ȷ���豸ID

#define DEV_PROTOCOL_CRYPTO_DATA_REQ          						0xA			///<	��ȡDas(Stun) info requset 
#define DEV_PROTOCOL_CRYPTO_DATA_RSP          						0xB			///<	��ȡDas(Stun) info response
#define DEV_PROTOCOL_STUN_REFRESHSESSIONKEY_I         				0xC			///<	��ȡStun info��sessionkey_I 
#define DEV_PROTOCOL_STUN_REFRESHSESSIONKEY_II          			0xD			///<	��ȡStun info��sessionkey_II 
#define DEV_PROTOCOL_STUN_REFRESHSESSIONKEY_III          			0xE			///<	��ȡStun info��sessionkey_III 
#define DEV_PROTOCOL_GET_SECRETKEY				          			0xF			///<	��֤�벻�Ϲ��豸��������secretkey

#define DEV_PROTOCOL_LBS_FORM_VERSION							0x01		///<	��LBS����Э����ʽ�汾��
#define DEV_PROTOCOL_LBS_LOW_TYPE_VERSION						0x01		///<	��LBS����Э�����Ͱ汾��
#define DEV_PROTOCOL_LBS_LOW_TYPE_VERSION_LICENSE				0x02		///<	��LBS����Э�����Ͱ汾�� --- ֧��license�����İ汾
#define DEV_PROTOCOL_LBS_HIGH_TYPE_VERSION						0x00		///<	��LBS����Э�����Ͱ汾��


#define DEV_PROTOCOL_LBS						0			///<	�ɹ�
#define DEV_PROTOCOL_LBS_SIGNCHECK_ERROR		1			///<	sign��֤ʧ��
#define DEV_PROTOCOL_LBS_ORDER_ERROR			2			///<	����ʱ��������
#define DEV_PROTOCOL_LBS_INVALID_DATA			3			///<	��Ч����
#define DEV_PROTOCOL_LBS_DEVID_INCONFORMITY		4			///<	�豸�ϴ���DEVID��ƽ̨��¼��һ�£������Ǵ����豸��
#define DEV_PROTOCOL_LBS_QUERY_AUTHCODE_ERROR	5			///<	��ѯ��֤��ʧ��
#define DEV_PROTOCOL_LBS_QUERY_AUTHCODE_REDIS	6			///<	��ѯredisʧ��
#define DEV_PROTOCOL_LBS_DEC_ERROR				7			///<	����ʧ��
#define DEV_PROTOCOL_LBS_ENC_ERROR				8			///<	����ʧ��
#define DEV_PROTOCOL_LBS_GETSTUN_ERROR			9			///<	��ȡdas��Ϣ��stun��Ϣʧ��


#define DEV_PROTOCOL_MQTT_SUCC								0			///<	MQTT �ɹ�
#define DEV_PROTOCOL_MQTT_NOSUPPORT_PROTOCOL_VERSION		1			///<	MQTT �����Ѿܾ�����֧�ֵ�Э��汾
#define DEV_PROTOCOL_MQTT_UNQUALIFIED_CLIENT_ID				2			///<	MQTT �����Ѿܾ������ϸ�Ŀͻ��˱�ʶ��
#define DEV_PROTOCOL_MQTT_SERVER_UNUSABLE					3			///<	MQTT �����Ѿܾ�������˲�����
#define DEV_PROTOCOL_MQTT_INVALID_USERNAME					4			///<	MQTT �����Ѿܾ�����Ч���û���������
#define DEV_PROTOCOL_MQTT_UNAUTHORIZED						5			///<	MQTT �����Ѿܾ���δ��Ȩ
#define DEV_PROTOCOL_MQTT_BLACKLIST							10			///<	�豸�����뵽������
#define DEV_PROTOCOL_MQTT_REDIRECT							11			///<	�豸�ỰʧЧ(��Ҫ�ض���)


#define	DAS_CMD_DOMAIN										1000			///<	�豸����
#define DAS_CMD_COMMON_FUN									1001			///<	ͨ�ù���

#define DAS_CMD_CENPLT2PUDOMAINCONFIG						0x0001		///<	ƽ̨�·���������ϵͳ����
#define DAS_CMD_CENPLT2PURISKCONFIG							0x0002		///<	ƽ̨�·����ָ��
#define DAS_CMD_CENPLT2PUOFFLINE							0x0003		///<	ƽ̨֪ͨ�豸����
// #define DAS_CMD_CENPLT2PUOFFLINEREQ							0x2805		///<	ƽ̨Ҫ���豸��������
// #define DAS_CMD_CENPLT2PUOFFLINERSP							0x2806		///<	ƽ̨Ҫ���豸������Ӧ
// #define	DAS_CMD_CENPLT2DASDEVICEKICKOUTREQ					0x301D		///<	ƽ̨��Das�������豸��������
// #define	DAS_CMD_CENPLT2DASDEVICEKICKOUTRSP					0x301E		///<	ƽ̨��Das�������豸������Ӧ
#define DAS_CMD_CENPLT2PUSETKEEPALIVETIMEREQ				0x3450		///<	ƽ̨���豸���������������
#define DAS_CMD_CENPLT2PUSETKEEPALIVETIMERSP				0x3451		///<	ƽ̨���豸�������������Ӧ
#define DAS_CMD_CENPLT2PUSETLBSDOMAINNAMEBYDASREQ			0x3479		///<	ƽ̨���豸�·��ض�������
#define DAS_CMD_CENPLT2PUSETLBSDOMAINNAMEBYDASRSP			0x347A		///<	ƽ̨���豸�·��ض�����Ӧ
#define DAS_CMD_CENPLT2PUSETLBSDOMAINNAMEREQ				0x491F		///<	ƽ̨���豸�·��ض�������
#define DAS_CMD_CENPLT2PUSETLBSDOMAINNAMERSP				0x4920		///<	ƽ̨���豸�·��ض�����Ӧ


#define DAS_CMD_COMM_DOMAIN_PU2CENPLTUPDATESTATUS					0x0001		///<	ͨ��״̬�ϱ�
#define	DAS_CMD_COMM_DOMAIN_CENPLT2PUSETSTATUS						0x0002		///<	ͨ��״̬����

#define	DAS_CMD_CENPLT2PUSETSWITCHENABLEREQ					0x490B		///<	ƽ̨���豸���ÿ���״̬����
#define DAS_CMD_CENPLT2PUSETSWITCHENABLERSP					0x490C		///<	ƽ̨���豸���ÿ���״̬��Ӧ

#define	DAS_CMD_CENPLT2PUQUERYSTATUSREQ						0x3061		///<	ƽ̨���豸��ȡ״̬����

#define	DAS_CMD_CENPLT2PUSETDEVPLANREQ						0x492B		///<	ƽ̨���豸���üƻ�����
#define	DAS_CMD_CENPLT2PUSETDEVPLANRSP						0x492C		///<	ƽ̨���豸���üƻ���Ӧ

#define DAS_CMD_CENPLT2PUSETCANARYTESTSTATUSREQ				0x4967		///<	ƽ̨���豸���ûҶȷ���״̬����
#define DAS_CMD_CENPLT2PUSETCANARYTESTSTATUSRSP				0x4968		///<	ƽ̨���豸���ûҶȷ���״̬��Ӧ

#define DAS_CMD_PU2CENPLTUPGRADEREQ							0x2863		///<	�豸��ƽ̨��ѯ��������	
#define DAS_CMD_PU2CENPLTUPGRADERSP							0x2864		///<	�豸��ƽ̨��ѯ������Ӧ
#define DAS_CMD_PU2CENPLTSETKEEPALIVETIMEREQ                0X3452      ///<    �豸��ƽ̨���������������
#define DAS_CMD_PU2CENPLTSETKEEPALIVETIMERSP                0X3453      ///<    �豸��ƽ̨�������������Ӧ

#define DEV_COMMON_CENPLT2PUSETKEYVALUEREQ					0x498D		///<	ƽ̨���豸���ü�ֵ��״̬����

#endif
/**
 * \file		mkernel_internal_error.h
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
*  \sa mkernel_internal_error
* 
*/

#ifndef H_MKERNEL_INTERNAL_ERROR_H_
#define H_MKERNEL_INTERNAL_ERROR_H_

typedef enum 
{
	mkernel_internal_succ = 0,							///<		�ɹ�
	mkernel_internal_no_start,							///<		SDKδ����
	mkernel_internal_haven_stop,						///<		SDK�Ѿ��رգ�����SDK��˵���Խ���ж������
	mkernel_internal_invald_call,						///<		������һ���Ƿ����ã������������
	mkernel_internal_das_need_reconnect,				///<		DAS��Ҫ����
	mkernel_internal_input_param_invalid,				///<		����Ĳ����Ƿ�
	mkernel_internal_lbs_connect_error,					///<		����lbs����
	mkernel_internal_mem_lack,							///<		���õ��ڴ治��
	mkernel_internal_net_gethostbyname_error,			///<		��������ʧ��
	mkernel_internal_create_sock_error,					///<		����socketʧ��
	mkernel_internal_net_connect_error,					///<		socket ����ʧ��
	mkernel_internal_net_connect_timeout,				///<		socket ���ӳ�ʱ
	mkernel_internal_net_send_error,					///<		lbs ���ʹ���
	mkernel_internal_net_read_error,					///<		lbs ���մ���
	mkernel_internal_net_read_error_request,			///<		���յ��������Ӧ
	mkernel_internal_net_socket_error,					///<		socket error
	mkernel_internal_net_socket_timeout,				///<		socket timeout
	mkernel_internal_net_socket_closed,					///<		socket closed
	mkernel_internal_net_send_buf_full,					///<		send buf full
	mkernel_internal_malloc_error,						///<		�����ڴ����
	mkernel_internal_json_parse_error,					///<		����json����
	mkernel_internal_get_error_json,					///<		����json���ģ�ָ���뱨�ĸ�ʽ��һ��
	mkernel_internal_json_format_error,					///<		json��ʽ��ʧ��
	mkernel_internal_xml_parse_error,					///<		����xml����
	mkernel_internal_get_error_xml,						///<		����xml���ģ�ָ���뱨�ĸ�ʽ��һ��
	mkernel_internal_rev_invalid_packet,				///<		���յ�һ���Ƿ�����

	mkernel_internal_force_offline,						///<		�豸��ǿ������		���յ���������ʾ΢�ں��ڲ��Ѿ�ֹͣ�����ˣ���Ҫ�����ɵ��ùرսӿ��ͷ���Դ
	mkernel_internal_force_domain_risk,					///<		�豸���򱻷�ص���
	mkernel_internal_force_cmd_risk,					///<		�豸ָ���ص���

	mkernel_internal_net_poll_err,						///<		poll error
	mkernel_internal_net_getsockopt_error,				///<		getsockopt
	mkernel_internal_net_socket_err,					///<		SO_ERROR
	mkernel_internal_net_poll_event_err,				///<		poll_event_err
	mkernel_internal_value_load_err,					///<		��ȡ����ʧ��
	mkernel_internal_value_save_err,					///<		��������ʧ��
	mkernel_internal_internal_err,						///<		�ڲ�����
	mkernel_internal_msg_len_overrange,                  ///<        ��Ϣ���ȳ�����Χ
	mkernel_internal_das_need_rebuild_session,

	mkernel_internal_call_mqtt_connect = 100,			///<		����MQTT ע��
	mkernel_internal_call_mqtt_sub_error,				///<		����MQTT ����topic
	mkernel_internal_call_mqtt_pub_error,				///<		����MQTT ����ʧ��
	mkernel_internal_call_mqtt_yield_error,				///<		����MQTT ����
	mkernel_internal_call_mqtt_disconnect,				///<		����MQTT ע��
	mkernel_internal_call_mqtt_buffer_too_short,		///<		����MQTT ����ռ�̫С

	mkernel_internal_call_coap_connect = 150,           ///<        ����COAP ע��
	mkernel_internal_call_coap_pub_error,               ///<        ����COAP ����ʧ��
	mkernel_internal_call_coap_parse_error,             ///<        ����COAP ����ʧ��
	mkernel_internal_call_coap_yield_error,             ///<        ����COAP ����
	mkernel_internal_call_coap_update_sessionkey_error, ///<        ����COAP ����sessionKey
	mkernel_internal_call_coap_udp_port_time_low_error, ///<        ����COAP UDP�˿ڼ��ʱ���С

	mkernel_internal_extend_id_error = 200,				///<		��չID����
	mkernel_internal_extend_unreg,						///<		��չδע��
	mkernel_internal_extend_full,						///<		��չע������
	mkernel_internal_extend_ready,						///<		��չ�Ѿ�ע��
	mkernel_internal_extend_no_find,					///<		��չδ�ҵ�
	mkernel_internal_common_ready,						///<		ͨ��ģ���Ѿ�ע���ˣ�ֻ֧��ע��һ�Σ�
	mkernel_internal_aes_input_len = 300,				///<		aes��������ݳ��Ȳ���
	mkernel_internal_casll_mbedtls_setdeckey_error,		///<		����������Կ����
	mkernel_internal_casll_mbedtls_crypt_error,			///<		���ü��ܽӿڴ���
	mkernel_internal_aes_padding_unmatched,				///<		aes padding��ʽ��ƥ��
	mkernel_internal_bscomptls_ecp_group_load_err,       ///< 
	mkernel_internal_bscomptls_ecdh_read_public_err,
	mkernel_internal_bscomptls_ecdh_calc_secret_err,
	

	mkernel_internal_queue_empty = 500,					///<		����Ϊ��
	mkernel_internal_queue_uninit,						///<		����δ��ʼ��
	mkernel_internal_queue_error,						///<		�����ڲ�����˳�����
	mkernel_internal_queue_full,						///<		������

	mkernel_internal_platform_appoint_error =600,		///<		���յ���Э��Լ����һ��
	mkernel_internal_sign_check_error,					///<		ǩ��У�����
	mkernel_internal_hmac_error,						///<		hmacǩ��ʧ��
	mkernel_internal_dec_error,							///<		����ʧ��	
	mkernel_internal_enc_error,							///<		����ʧ��
	mkernel_internal_random1_check_error,				///<		�����1У�����
	mkernel_internal_hmac_compare_error,						///<		hmacǩ��ʧ��

	mkernel_internal_platform_error = 10000,					///<	ƽ̨���ش���
	mkernel_internal_platform_lbs_signcheck_error,				///<	sign��֤ʧ��,��֤�벻ƥ��									
	mkernel_internal_platform_lbs_order_error,					///<	����ʱ��������
	mkernel_internal_platform_invalid_data,						///<	��Ч����
	mkernel_internal_platform_devid_inconformity,				///<	�豸�ϴ���DEVID��ƽ̨��¼��һ�£������Ǵ����豸�� ��������devid
	mkernel_internal_platform_query_authcode_error,				///<	��ѯ��֤��ʧ��,û����̨�豸
	mkernel_internal_platform_query_authcode_redis,				///<	��ѯredisʧ��
	mkernel_internal_platform_dec_error,						///<	����ʧ��
	mkernel_internal_platform_enc_error	,						///<	����ʧ��
	mkernel_internal_platform_getstun_error,				 	///<	��ȡdas��Ϣ��stun��Ϣʧ��
	mkernel_internal_platform_masterkey_invalid,				///<	maskeyʧЧ
	mkernel_internal_platform_stun_sessionkey_inconformity,		///<	��ȡStun��Ϣ �豸��������session key��һ��
	mkernel_internal_platform_stun_process_invalid,				///<	��ȡStun��Ϣ ���̲���
	mkernel_internal_platform_das_process_invalid,				///<	��ȡDas��Ϣ ���̲���

	mkernel_internal_platform_lbs_sign_decrypt_err,             ///<	HMac384ǩ��У�����
	mkernel_internal_platform_lbs_gen_keys_err,                 ///<    lbs������Կ����
	mkernel_internal_platform_lbs_ecdh_check_err,               ///<    lbs ecdh
	mkernel_internal_platform_lbs_check_ecdh_sign_fail,   
	mkernel_internal_platform_lbs_check_sessionkey_fail,        ///<   sessionkey��֤ʧ��
	mkernel_internal_platform_lbs_sign_check_fail,              ///<   auth_iУ��signʧ�ܡ�����device idУ��signʧ��

    mkernel_internal_platform_lbs_auth_type_need_rematch,       ///<   �豸��ƽ̨����֤���Ͳ�ƥ�䣨��ǰ��ƥ�䣬������ƥ�䣩
    mkernel_internal_platform_lbs_auth_type_match_fail,         ///<   �豸��ƽ̨����֤����ƥ��ʧ�ܣ��޷�ƥ�䣩

	mkernel_internal_platform_secretkey_decrypt_fail = 10100,	///<	����ʧ��
	mkernel_internal_platform_secretkey_overflow_windows,		///<	�����ڴ�����
	mkernel_internal_platform_secretkey_no_user,				///<	�豸δ���û�
	mkernel_internal_platform_secretkey_serial_not_exist,		///<	�豸������
	mkernel_internal_platform_secretkey_again,					///<	�豸�ظ�����
	mkernel_internal_platform_error_end = 11000,

	mkernel_internal_mqtt_error_begin = 11000,
 	mkernel_internal_mqtt_nosupport_protocol_version,			///<	MQTT �����Ѿܾ�����֧�ֵ�Э��汾
 	mkernel_internal_mqtt_unqualified_client_id,				///<	MQTT �����Ѿܾ������ϸ�Ŀͻ��˱�ʶ��
 	mkernel_internal_mqtt_server_unusable,						///<	MQTT �����Ѿܾ�������˲�����
 	mkernel_internal_mqtt_invalid_username,						///<	MQTT �����Ѿܾ�����Ч���û���������
 	mkernel_internal_mqtt_unauthorized,							///<	MQTT �����Ѿܾ���δ��Ȩ
 	mkernel_internal_mqtt_blacklist =	11010,					///<	�豸�����뵽������
 	mkernel_internal_mqtt_redirect =	11011,					///<	�豸�ỰʧЧ
	mkernel_internal_mqtt_session_exist =	11012,				///<	�豸�Ự�Ѵ���
	mkernel_internal_mqtt_error_end = 12000,
}mkernel_internal_error;

#endif


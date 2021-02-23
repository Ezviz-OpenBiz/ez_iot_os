/**
 * \file		net_platform.c
 *
 * \brief		������ؽӿڵ�Windowsʵ��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		xurongjun
 *
 * \date		2018/6/27
 */

#include "net_platform_wrapper.h"
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include "base_typedef.h"
#include "ezdev_sdk_kernel_struct.h"
#include <../src/mkernel_internal_error.h>

/** 
 *  \brief		�ж��Ƿ��ǺϷ���IP��ַ
 *  \method		isIPAddress
 *  \param[in] 	s IP��ַ
 *  \return 	�ɹ�����1 ʧ�ܷ���0
 */
char isIPAddress(const char *s)  
{  
	const char *pChar;  
	char rv = 1;  
	int tmp1, tmp2, tmp3, tmp4, i;  

	while( 1 )  
	{  
		i = sscanf(s, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);  

		if( i != 4 )  
		{  
			rv = 0;  
			break;  
		}  

		if( (tmp1 > 255) || (tmp2 > 255) || (tmp3 > 255) || (tmp4 > 255) )  
		{  
			rv = 0;  
			break;  
		}  

		for( pChar = s; *pChar != 0; pChar++ )  
		{  
			if( (*pChar != '.')  
				&& ((*pChar < '0') || (*pChar > '9')) )  
			{  
				rv = 0;  
				break;  
			}  
		}  
		break;  
	}  

	return rv;  
}

/** 
 *  \brief		�ж��Ƿ��ǺϷ���IP��ַ
 *  \method		parse_host
 *  \param[in] 	host ������IP
 *  \param[out] ����IP�Ļ�����
 *  \return 	�ɹ�����IP��ַ ʧ�ܷ���INADDR_NONE
 */
unsigned long parse_host(const char* host, char szRealIp[ezdev_sdk_ip_max_len])
{
	unsigned long raddr = INADDR_NONE;
	struct addrinfo* ailist = NULL, * aip = NULL;
	struct addrinfo hint;
	int ret = 0;
	char* szParseIp = NULL;
	memset(&hint, 0, sizeof(hint));

	if (host == NULL || strlen(host) == 0) 
	{
		return INADDR_NONE;
	}

	if (isIPAddress(host))
	{
		hint.ai_flags = AI_NUMERICHOST;
	}
	ret = getaddrinfo(host, NULL, &hint, &ailist);
	if (ret != 0)
	{
		return INADDR_NONE;
	}
	if (ailist == NULL) 
	{
		return INADDR_NONE;
	}

	for (aip = ailist; aip != NULL; aip = aip->ai_next) 
	{
		raddr = ((struct sockaddr_in*)aip->ai_addr)->sin_addr.s_addr;

		szParseIp = inet_ntop(AF_INET, &((struct sockaddr_in*)aip->ai_addr)->sin_addr, szRealIp, ezdev_sdk_ip_max_len);

		if (strlen(szParseIp) >= ezdev_sdk_ip_max_len)
		{
			strncpy(szRealIp, szParseIp, ezdev_sdk_ip_max_len - 1);
		}
		else
		{
			strncpy(szRealIp, szParseIp, strlen(szParseIp));
		}
		printf("%s\n", szParseIp);
		break;
	}
	freeaddrinfo(ailist);
	return raddr;
}

/** 
 *  \brief		�������ӵĴ���,Ĭ��ΪTCPЭ��
 *  \method		net_create
 *  \param[in] 	nic_name	�������ƣ����nic_name��Ϊ�ջ�ָ��ĵ�ַ����Чֵ��������socket�����������
 *  \return 	�ɹ������������������� ʧ�ܷ���NULL
 */
ezdev_sdk_net_work net_create(char* nic_name)
{
	int arg = 1;
	win_net_work* winnet_work = NULL;
	const int opt = 1400;
	int ret = 0;
	winnet_work = (win_net_work*)malloc(sizeof(win_net_work));
	if (winnet_work == NULL)
	{
		return NULL;
	}

	winnet_work->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (winnet_work->socket_fd == -1)
	{
		free(winnet_work);
		return NULL;
	}

	ret = setsockopt(winnet_work->socket_fd , IPPROTO_TCP, TCP_MAXSEG, &opt, sizeof(opt));
	if (ret < 0) 
	{
		printf("set socket opt, TCP_MAXSEG error\n");
	}

	return (ezdev_sdk_net_work)winnet_work;
}

/** 
 *  \brief		��������
 *  \method		net_connect
 *  \param[in] 	net_work	��������������
 *  \param[in]	server_ip	��������ַ
 *  \param[in]	timeout_ms	���ӳ�ʱʱ��
 *  \param[out]	szRealIp	����IP
 *  \return 	�ɹ�0 ʧ�ܷ��ض�Ӧ������
 */
mkernel_internal_error net_connect(ezdev_sdk_net_work net_work, const char* server_ip, int server_port,  int timeout_ms, char szRealIp[ezdev_sdk_ip_max_len])
{
	SOCKADDR_IN dst_addr;
	struct timeval cnt_timeout;
	fd_set cnt_fdset; 
	int arg = 0;
	int return_value = 0;
	int winerror = 0;
	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work)
	{
		return mkernel_internal_input_param_invalid;
	}

	ioctlsocket(winnet_work->socket_fd, FIONBIO, &arg);

	dst_addr.sin_family=AF_INET;
	dst_addr.sin_addr.S_un.S_addr=parse_host(server_ip, szRealIp);
	dst_addr.sin_port=htons(server_port);
	if(INADDR_NONE == dst_addr.sin_addr.S_un.S_addr)
		return mkernel_internal_net_gethostbyname_error;

	if (connect(winnet_work->socket_fd, &dst_addr, sizeof(dst_addr)) == SOCKET_ERROR && WSAGetLastError() != WSAEINPROGRESS && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		winerror = WSAGetLastError();
		return mkernel_internal_net_connect_error;
	}
	
	FD_ZERO(&cnt_fdset); 
	FD_SET(winnet_work->socket_fd, &cnt_fdset); 

	cnt_timeout.tv_sec = timeout_ms/1000; 
	cnt_timeout.tv_usec = (timeout_ms%1000)*1000;

	return_value = select(winnet_work->socket_fd+1,0,&cnt_fdset,0,&cnt_timeout);
	if (return_value <= 0)
	{
		return mkernel_internal_net_connect_timeout;
	}

	return 0;
}

/** 
 *  \brief		�����������
 *  \method		net_read
 *  \param[in] 	net_work			��������������
 *  \param[in]	read_buf			���ջ�����
 *  \param[in]	read_buf_maxsize	�������ݵĴ�С
 *  \param[out]	read_timeout_ms		�������ݳ�ʱʱ��
 *  \return 	�ɹ�0 ʧ�ܷ��ض�Ӧ������
 */
mkernel_internal_error net_read(ezdev_sdk_net_work net_work, unsigned char* read_buf, int read_buf_maxsize, int read_timeout_ms)
{
	int rev_size = 0;
	int rev_total_size = 0;
	int return_value = 0;
	fd_set read_fd;
	struct timeval tmval = {read_timeout_ms/1000, (read_timeout_ms%1000)*1000};

	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work)
	{
		return mkernel_internal_input_param_invalid;
	}

	do
	{
//		fd_set read_fd;
	//	struct timeval tmval = {read_timeout_ms/1000, (read_timeout_ms%1000)*1000};
		FD_ZERO(&read_fd);
		FD_SET(winnet_work->socket_fd, &read_fd);

		return_value = select(winnet_work->socket_fd + 1, &read_fd, NULL, NULL, &tmval);
		if (return_value < 0)
		{
			//socket error
			return mkernel_internal_net_socket_error;
		}
		else if (return_value == 0)
		{
			//timeout
			return mkernel_internal_net_socket_timeout;
		}
		
		rev_size = recv(winnet_work->socket_fd, read_buf + rev_total_size, read_buf_maxsize - rev_total_size, 0);
		if (rev_size < 0)
		{
			//socket error
			return mkernel_internal_net_socket_error;
		}
		else if (rev_size == 0)
		{
			// socket close
			return mkernel_internal_net_socket_closed;
		}
		rev_total_size += rev_size;
		
	}while(rev_total_size < read_buf_maxsize);


	return mkernel_internal_succ;
}

/** 
 *  \brief		���緢������
 *  \method		net_write
 *  \param[in] 	net_work			��������������
 *  \param[in]	write_buf			���ͻ�����
 *  \param[in]	write_buf_size		�������ݵĴ�С
 *  \param[in]	read_timeout_ms		�������ݳ�ʱʱ��
 *  \param[out]	real_write_buf_size	ʵ�ʷ��͵Ĵ�С
 *  \return 	�ɹ�0 ʧ�ܷ��ض�Ӧ������
 */
mkernel_internal_error net_write(ezdev_sdk_net_work net_work, unsigned char* write_buf, int write_buf_size, int send_timeout_ms, int* real_write_buf_size)
{
	int send_size = 0;
	int send_total_size = 0;

	fd_set write_fd;
	struct timeval tmval = {send_timeout_ms/1000, (send_timeout_ms%1000)*1000};

	int return_value = 0;

	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work || NULL == real_write_buf_size)
	{
		return mkernel_internal_input_param_invalid;
	}
	
	do
	{
		FD_ZERO(&write_fd);
		FD_SET(winnet_work->socket_fd, &write_fd);

		return_value = select(winnet_work->socket_fd + 1,NULL , &write_fd, NULL, &tmval);
		if (return_value < 0)
		{
			//socket error
			return mkernel_internal_net_socket_error;
		}
		else if (return_value == 0)
		{
			//timeout
			return mkernel_internal_net_socket_timeout;
		}

		send_size = send(winnet_work->socket_fd, write_buf, write_buf_size, 0);
		if (send_size == -1)
		{
			//socket error
			return mkernel_internal_net_socket_error;
		}
		
		*real_write_buf_size = send_size;

	} while(0);

	return mkernel_internal_succ;
}

/** 
 *  \brief		�Ͽ���������
 *  \method		net_disconnect
 *  \param[in] 	net_work			��������������
 */
void net_disconnect(ezdev_sdk_net_work net_work)
{
	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work)
	{
		return;
	}
	closesocket(winnet_work->socket_fd);
	winnet_work->socket_fd = 0;
}

/** 
 *  \brief		������������
 *  \method		net_destroy
 *  \param[in] 	net_work			��������������
 */
void net_destroy(ezdev_sdk_net_work net_work)
{
	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work)
	{
		return;
	}
	free(winnet_work);
}

int net_getsocket(ezdev_sdk_net_work net_work)
{
	win_net_work* winnet_work = (win_net_work*)net_work;
	if (NULL == winnet_work)
	{
		return -1;
	}
	

	return winnet_work->socket_fd;
}
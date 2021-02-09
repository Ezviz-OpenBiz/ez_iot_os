#include "ezdev_sdk_kerne_queuel.h"
#include "sdk_kernel_def.h"
#include "ezdev_sdk_kernel_platform.h"
#include "ezdev_sdk_kernel_extend.h"
#include "bscJSON.h"

EZDEV_SDK_KERNEL_PLATFORM_INTERFACE
EXTERN_QUEUE_BASE_FUN
EZDEV_SDK_KERNEL_EXTEND_INTERFACE

queque_submsg g_queue_submsg;					///<	���շ�������Ϣ���ַ�������ģ����ϲ�Ӧ��
queque_pubmsg_exchange g_queue_pubmsg_exchange; ///<	��������ģ����ϲ�Ӧ����Ϣ������������
queque_inner_cb_notic g_queue_inner_cb_notic;   ///<	���ձ��ع㲥���ַ�������ģ����ϲ�Ӧ��

queque_submsg_v3 g_queue_submsg_v3;					///<	���շ�������ϢV3Э��, �ַ�������ģ����ϲ�Ӧ��
queque_pubmsg_exchange_v3 g_queue_pubmsg_exchange_v3; ///<	�����ϲ�Ӧ����ϢV3Э��,����������

QUEUE_INIT(submsg)			   ///<	չ����Ϊinit_queue_submsg(EZDEV_SDK_UINT8 max_size)����
QUEUE_INIT(pubmsg_exchange)    ///<	չ����Ϊinit_queue_pubmsg_exchange(EZDEV_SDK_UINT8 max_size)����
QUEUE_INIT(inner_cb_notic)     ///<	չ����Ϊinit_queue_inner_cb_notic(EZDEV_SDK_UINT8 max_size)����

QUEUE_INIT(submsg_v3)	       ///<	չ����Ϊinit_queue_submsg_v3(EZDEV_SDK_UINT8 max_size)����
QUEUE_INIT(pubmsg_exchange_v3) ///<	չ����Ϊinit_queue_pubmsg_exchange_v3(EZDEV_SDK_UINT8 max_size)����

QUEUE_FINI(submsg)			   ///<	չ����Ϊfini_queue_submsg()����
QUEUE_FINI(pubmsg_exchange)    ///<	չ����Ϊfini_queue_pubmsg_exchange()����
QUEUE_FINI(inner_cb_notic)     ///<	չ����Ϊfini_queue_inner_cb_notic()����

QUEUE_FINI(submsg_v3)			   ///<	չ����Ϊfini_queue_submsg_v3()����
QUEUE_FINI(pubmsg_exchange_v3)    ///<	չ����Ϊfini_queue_pubmsg_exchange_v3()����

QUEUE_POP(submsg)		       ///<	չ����Ϊpop_queue_submsg(ezdev_sdk_kernel_submsg**)����
QUEUE_POP(pubmsg_exchange)     ///<	չ����Ϊpop_queue_pubmsg_exchange(ezdev_sdk_kernel_pubmsg_exchange**)����
QUEUE_POP(inner_cb_notic)      ///<	չ����Ϊpop_queue_inner_cb_notic(ezdev_sdk_kernel_inner_cb_notic**)����
QUEUE_GET(pubmsg_exchange)     ///<	չ����Ϊget_queue_pubmsg_exchange(ezdev_sdk_kernel_pubmsg_exchange**)����

QUEUE_POP(submsg_v3)		       ///<	չ����Ϊpop_queue_submsg_v3(ezdev_sdk_kernel_submsg_v3**)����
QUEUE_POP(pubmsg_exchange_v3)     ///<	չ����Ϊpop_queue_pubmsg_exchange_v3(ezdev_sdk_kernel_pubmsg_exchange_v3**)����
QUEUE_GET(pubmsg_exchange_v3)     ///<	չ����Ϊget_queue_pubmsg_exchange_v3(ezdev_sdk_kernel_pubmsg_exchange_v3**)����

QUEUE_PUSH(submsg)			   ///<	չ����Ϊpush_queue_submsg(ezdev_sdk_kernel_submsg**)����
QUEUE_PUSH(pubmsg_exchange)    ///<	չ����Ϊpush_queue_pubmsg_exchange(ezdev_sdk_kernel_pubmsg_exchange**)����
QUEUE_PUSH(inner_cb_notic)     ///<	չ����Ϊpush_queue_inner_cb_notic(ezdev_sdk_kernel_inner_cb_notic**)����

QUEUE_PUSH(submsg_v3)			   ///<	չ����Ϊpush_queue_submsg_v3(ezdev_sdk_kernel_submsg_v3**)����
QUEUE_PUSH(pubmsg_exchange_v3)    ///<	չ����Ϊpush_queue_pubmsg_exchange_v3(ezdev_sdk_kernel_pubmsg_exchangeV3**)����


QUEUE_PUSH_HEAD(submsg)			 ///<	չ����Ϊpush_queue_head_submsg(ezdev_sdk_kernel_submsg**)����
QUEUE_PUSH_HEAD(pubmsg_exchange) ///<	չ����Ϊpush_queue_head_pubmsg_exchange(ezdev_sdk_kernel_pubmsg_exchange**)����
QUEUE_PUSH_HEAD(inner_cb_notic)  ///<	չ����Ϊpush_queue_head_inner_cb_notic(ezdev_sdk_kernel_inner_cb_notic**)����

QUEUE_PUSH_HEAD(submsg_v3)			 ///<	չ����Ϊpush_queue_head_submsg_v3(ezdev_sdk_kernel_submsg_v3**)����
QUEUE_PUSH_HEAD(pubmsg_exchange_v3) ///<	չ����Ϊpush_queue_head_pubmsg_exchange_v3(ezdev_sdk_kernel_pubmsg_exchange_v3**)����

mkernel_internal_error init_queue(EZDEV_SDK_UINT16 sub_max_size, EZDEV_SDK_UINT16 pub_max_size, EZDEV_SDK_UINT16 inner_max_size)
{
	init_queue_submsg(sub_max_size);
	init_queue_pubmsg_exchange(pub_max_size);
	init_queue_inner_cb_notic(inner_max_size);

	init_queue_submsg_v3(sub_max_size);
	init_queue_pubmsg_exchange_v3(pub_max_size);
	return mkernel_internal_succ;
}

void fini_queue()
{
	ezdev_sdk_kernel_inner_cb_notic *ptr_inner_cb_notic = NULL;
	clear_queue_submsg();
	clear_queue_submsg_v3();
	clear_queue_pubmsg_exchange_v3();
	/**
	 * \brief   �����ڲ������ڴ���Ҫ�ͷ�
	 */
	do
	{
		ptr_inner_cb_notic = NULL;
		if (mkernel_internal_queue_empty == pop_queue_inner_cb_notic(&ptr_inner_cb_notic) || ptr_inner_cb_notic == NULL)
		{
			break;
		}

		destroy_inner_cb_notic(ptr_inner_cb_notic);
	} while (1);

	fini_queue_submsg();
	fini_queue_pubmsg_exchange();
	fini_queue_inner_cb_notic();

	fini_queue_submsg_v3();
	fini_queue_pubmsg_exchange_v3();
}

extern void destroy_inner_cb_notic(ezdev_sdk_kernel_inner_cb_notic *ptr_inner_cb_notic)
{
	sdk_runtime_err_context *rt_err_ctx = NULL;
	sdk_send_msg_ack_context *ack_ctx = NULL;
	sdk_send_msg_ack_context_v3 *ack_ctx_v3 = NULL;

	do
	{
		if (NULL == ptr_inner_cb_notic)
			break;

		if (sdk_kernel_event_runtime_err != ptr_inner_cb_notic->cb_event.event_type || NULL == ptr_inner_cb_notic->cb_event.event_context)
			break;

		rt_err_ctx = (sdk_runtime_err_context *)(ptr_inner_cb_notic->cb_event.event_context);

		if (extend_cb_event == ptr_inner_cb_notic->cb_type &&
			sdk_kernel_event_runtime_err == ptr_inner_cb_notic->cb_event.event_type)
		{
			if (TAG_MSG_ACK == rt_err_ctx->err_tag)
			{
				ack_ctx = (sdk_send_msg_ack_context *)rt_err_ctx->err_ctx;
			}
			else if(TAG_MSG_ACK_V3 == rt_err_ctx->err_tag)
			{
				ack_ctx_v3 = (sdk_send_msg_ack_context_v3 *)rt_err_ctx->err_ctx;
			}
		}

		if (NULL != ack_ctx)
		{
			if (NULL != ack_ctx->externel_ctx)
				free(ack_ctx->externel_ctx);

			free(ack_ctx);
		}

		if (NULL != ack_ctx_v3)
		{
			free(ack_ctx_v3);
		}
		
	} while (0);

	if (NULL != ptr_inner_cb_notic)
	{
		if (NULL != ptr_inner_cb_notic->cb_event.event_context)
		{
			free(ptr_inner_cb_notic->cb_event.event_context);
			ptr_inner_cb_notic->cb_event.event_context = NULL;
		}

		free(ptr_inner_cb_notic);
	}
}

/*****************
ezdev_sdk_kernel_error pop_queue_submsg(ezdev_sdk_kernel_submsg** submsg)
{
	if (g_ezdev_sdk_kernel_queue.submsg_queque_head == NULL)
	{
		g_ezdev_sdk_kernel_queue.submsg_queque_tail = NULL;
		g_ezdev_sdk_kernel_queue.submsg_size = 0;
		return ezdev_sdk_kernel_queue_empty;
	}

	*submsg = g_ezdev_sdk_kernel_queue.submsg_queque_head->msg
	g_ezdev_sdk_kernel_queue.submsg_queque_head = g_ezdev_sdk_kernel_queue.submsg_queque_head->next;

	if (g_ezdev_sdk_kernel_queue.submsg_queque_head == NULL)
	{
		g_ezdev_sdk_kernel_queue.submsg_queque_tail = NULL;
		g_ezdev_sdk_kernel_queue.submsg_size = 0;
	}
	else
	{
		g_ezdev_sdk_kernel_queue.submsg_size--;
	}

	return ezdev_sdk_kernel_succ;
}

ezdev_sdk_kernel_error push_queue_submsg(ezdev_sdk_kernel_submsg* submsg)
{
	ezdev_sdk_kernel_submsg_queque* submsg_queque_element = NULL;
	if (g_ezdev_sdk_kernel_queue.submsg_size >= g_ezdev_sdk_kernel_queue.submsg_maxsize)
	{
		return ezdev_sdk_kernel_queue_full;
	}
	submsg_queque_element = (ezdev_sdk_kernel_submsg_queque*)malloc(sizeof(ezdev_sdk_kernel_submsg_queque);
	if (submsg_queque_element == NULL)
	{
		return ezdev_sdk_kernel_malloc_error;
	}
	submsg_queque_element->msg = submsg;
	submsg_queque_element->next = NULL;
	if (g_ezdev_sdk_kernel_queue.pubmsg_queque_tail == NULL || g_ezdev_sdk_kernel_queue.pubmsg_queque_head)
	{
		g_ezdev_sdk_kernel_queue.pubmsg_queque_head = submsg_queque_element;
		g_ezdev_sdk_kernel_queue.submsg_queque_tail = submsg_queque_element;
	}
	else
	{
		g_ezdev_sdk_kernel_queue.submsg_queque_tail->next = submsg_queque_element;
		g_ezdev_sdk_kernel_queue.submsg_queque_tail = submsg_queque_element;
	}
	g_ezdev_sdk_kernel_queue.submsg_size++;
	return ezdev_sdk_kernel_succ;
}
***********/
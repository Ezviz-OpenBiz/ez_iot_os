#ifndef H_PLATFORM_DEFINE_H_
#define H_PLATFORM_DEFINE_H_


#define LOG_PLATFORM_INTERFACE extern void log_print_error(int sdk_error, int othercode, const char * buf); \
	extern void log_print_warn(int sdk_error, int othercode, const char * buf); \
	extern void log_print_info(int sdk_error, int othercode, const char * buf); \
	extern void log_print_debug(int sdk_error, int othercode, const char * buf); \
	extern void log_print_trace(int sdk_error, int othercode, const char * buf); \
	extern void log_print(const char *fmt, ...);


#define NET_PLATFORM_INTERFACE	 \
	extern ezdev_sdk_net_work net_create(); \
	extern ezdev_sdk_kernel_error net_connect(ezdev_sdk_net_work net_work, const char* server_ip, EZDEV_SDK_INT32 server_port,  EZDEV_SDK_INT32 timeout, char szRealIp[ezdev_sdk_ip_max_len]); \
	extern ezdev_sdk_kernel_error net_read(ezdev_sdk_net_work net_work, unsigned char* read_buf, EZDEV_SDK_INT32 read_buf_maxsize, EZDEV_SDK_INT32 read_timeout); \
	extern ezdev_sdk_kernel_error net_write(ezdev_sdk_net_work net_work, unsigned char* write_buf, EZDEV_SDK_INT32 write_buf_size, EZDEV_SDK_INT32 send_timeout); \
	extern void net_disconnect(ezdev_sdk_net_work net_work); \
	extern void net_destroy(ezdev_sdk_net_work net_work); \
	int net_getsocket(ezdev_sdk_net_work net_work);


#define EZDEVSDK_CONFIG_INTERFACE  \
	extern int get_devinfo_fromconfig(const char* path, char* devinfo_context, int devinfo_context_len); \
	extern int set_file_value(const char* path, unsigned char* keyvalue, int keyvalue_size); \
	extern int get_file_value(const char* path, unsigned char* keyvalue, int keyvalue_maxsize);


#define TIME_PLATFORM_INTERFACE	\
	extern ezdev_sdk_time Platform_TimerCreater(); \
	extern char Platform_TimeIsExpired_Bydiff(ezdev_sdk_time time, EZDEV_SDK_UINT32 time_ms); \
	extern char Platform_TimerIsExpired(ezdev_sdk_time time); \
	extern void Platform_TimerCountdownMS(ezdev_sdk_time time, EZDEV_SDK_UINT32 timeout); \
	extern void Platform_TimerCountdown(ezdev_sdk_time time, EZDEV_SDK_UINT32 timeout); \
	extern EZDEV_SDK_UINT32 Platform_TimerLeftMS(ezdev_sdk_time time); \
	extern void Platform_TimeDestroy(ezdev_sdk_time time);
          
          
#define MUTEX_PLATFORM_INTERFACE	\
	extern ezdev_sdk_mutex sdk_platform_thread_mutex_create();	\
	extern void sdk_platform_thread_mutex_destroy(ezdev_sdk_mutex ptr_mutex);	\
	extern int sdk_platform_thread_mutex_lock(ezdev_sdk_mutex ptr_mutex);	\
	extern int sdk_platform_thread_mutex_unlock(ezdev_sdk_mutex ptr_mutex);

	
#endif //H_PLATFORM_DEFINE_H_
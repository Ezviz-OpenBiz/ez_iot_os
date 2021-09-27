
/*******************************************************************************
 * Copyright © 2017-2021 Ezviz Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *******************************************************************************/
#ifndef _THFACTORY_H
#define _THFACTORY_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct thfactory_s thfactory_t;

    enum
    {
        THREAD_KEEPALIVE = (0x01) ///< 工作线程长时间存活
    };

    struct thfactory_s
    {
        int m_binited;           ///< 初始化标志位
        int m_max_threads;       ///< 最大工作线程数量
        int m_max_task_num;      ///< 最大任务队列大小
        int m_threads_keepalive; ///< 工作线程是否长时间存活，长活线程没有任务时会阻塞等待，短活线程会退出。
        int m_threads_on_pause;  ///< 工作线程被暂停

        void **m_threads;                   ///< 线程句柄队列
        void *m_jobqueue;                   ///< 任务队列
        volatile int m_num_threads_alive;   ///< 当前存活的线程数量
        volatile int m_num_threads_working; ///< 当前处于工作状态的线程数量
        ez_mutex_t m_thcount_lock;     ///< 线程锁

        int (*init)(thfactory_t *thiz, int num_threads, int flags, int task_num);
        int (*add_task)(thfactory_t *thiz, int (*func_ptr)(void *), void *arg_ptr, void (*arg_free_func)(void *));
        int (*pause)(thfactory_t *thiz);
        int (*resume)(thfactory_t *thiz);
        int (*num_threads_working)(thfactory_t *thiz);
        void (*final)(thfactory_t *thiz);
    };

    thfactory_t *thfactory_new(void);
	
    void thfactory_del(thfactory_t *thiz);

#ifdef __cplusplus
}
#endif

#endif
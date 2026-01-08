/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : event_os.h
 * @author : SY7_yellow
 * @brief  : 事件操作系统模块
 * @date   : 2023-10-3
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-10-02     <td>1.1         <td>SY7_yellow  <td>创建初始版本
 * <tr><td>2023-10-03     <td>1.2-alpha   <td>SY7_yellow  <td>添加事件驱动
 * <tr><td>2023-11-01     <td>1.3         <td>SY7_yellow  <td>添加cpu使用率
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * ** 操作系统特点 **
 * 1. 该操作系统基于协作式事件驱动内核,在同优先级任务中会等待任务执行完毕退出后才
 * 会进入下一个任务,所以请不要在任务中写死循环.
 * 2. 高优先级任务会抢占低优先级任务,并且高优先级任务是在中断中执行的,所以高优先级
 * 任务不要占用太多资源.
 * 3. 该操作系统定时任务调用基于Event_OS_GetTick()来提供时基如果没有配置时基需要,
 * 使用一个1ms定时器创建时基.
 * 4. 全局的事件队列,相比每个任务一个事件队列极大的节约了内存空间.
 * 5. 可任意裁切,极其轻量,最小尺寸(仅保留任务定时调用)仅550Byte(o3优化).使用事件
 * 驱动不使用堆,在2事件2任务的情况下仅1.27KB(o3优化).
 * ** 如何使用 **
 * 1. 在进行任何操作系统操作前请调用Event_OS_Init()进行初始化.
 * 2. 若要使用优先级需创建一个最低中断优先级的1ms定时器,将Event_OS_Callback注册进
 * 该定时器中断回调函数.若不需要忽略这条.
 * 3. 初始化完毕后执行Event_OS_Run,启动事件操作系统.运行所有任务.
 * 4. 注册任务请调用Event_OS_Task_Register,传入相应参数.注册后任务默认为关闭状态,
 * 需要调用Event_OS_Task_Start或者Event_OS_Run启动任务.
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef _EVENT_OS_H_
#define _EVENT_OS_H_
#include "SEML_common.h"
#define USE_EVENT_DRIVEN 0
#define USE_HEAP 0
#define USE_CPU_UTILIZATION 1
#if USE_EVENT_DRIVEN
#include "queue.h"
#endif
#if USE_HEAP
#include "memory.h"
#endif
/**
 * @addtogroup 事件操作系统
 * @{
 */
/**
 * @brief 任务状态
 */
typedef enum
{
	EOS_Stop, /**< 任务停止 */
	EOS_Run		/**< 任务运行 */
} Event_OS_Run_Status_t;

/**
 * @brief 任务优先级
 */
typedef enum
{
	EOS_Low_Priority, /**< 低优先级 */
	EOS_High_Priority /**< 高优先级 */
} Event_OS_Priority_t;

#if USE_EVENT_DRIVEN
/**
 * @brief 事件
 */
typedef struct
{
	uint16_t id;
	struct
	{
		uint16_t broadcast : 1;
	} flag;
#if USE_HEAP
	void *data;
	uint16_t longth;
#endif
} Event_t;
#endif

/**
 * @brief 任务句柄
 */
typedef struct
{
	struct
	{
		uint16_t task_logout : 1;							/**< 任务准备注销 */
		uint16_t initialized : 1;							/**< 任务已初始化 */
		Event_OS_Run_Status_t run_status : 1; /**< 任务运行状态 */
		Event_OS_Priority_t priority : 1;			/**< 任务优先级 */
	} flag;
	struct
	{
		uint32_t timeout_tick;	 /**< 定时器计数器 */
		uint32_t timeout_buckup; /**< 定时器重装载值 */
	} timer;
	struct
	{
		void (*Constructor)(void); /**< 任务构造函数 */
		void (*Event_Task)(void *config);	 /**< 任务主函数 */
		void (*Destructor)(void);	 /**< 任务析构函数 */
	} callback;
#if USE_EVENT_DRIVEN
	Event_t event;
#endif
	void *next; /**< 下一个任务句柄 */
} Event_Task_Handle_t;

/**
 * @brief 任务回调函数
 */
typedef void (*Event_Task_t)(Event_Task_Handle_t const *const config);

/**
 * @brief 任务构造函数
 */
typedef void (*Task_Constructor_t)(void);

/**
 * @brief 任务析构函数
 */
typedef void (*Task_Destructor_t)(void);

#if USE_EVENT_DRIVEN
/**
 * @brief 最大事件数量
 */
#define EVENT_MAX_NUM 10
/**
 * @brief 最大事件订阅数量
 */
#define EVENT_SUB_MAX_NUM 5
#endif
#if USE_HEAP
/**
 * @brief 堆大小
 */
#define HEAP_SIZE 32768
#endif
/**
 * @addtogroup 任务操作函数
 * @{
 */
/**
 * @brief 注册任务
 * @param[out] task_handle 任务句柄
 * @param[in] loop_delay 循环延时,该传入0不使能定时器
 * @param[in] priority 任务优先级
 * @param[in] Constructor 任务构造函数
 * @param[in] Event_Task 任务回调函数
 * @param[in] Destructor 任务析构函数
 */
void Event_OS_Task_Register(Event_Task_Handle_t *task_handle, uint32_t loop_delay, Event_OS_Priority_t priority, Task_Constructor_t Constructor, Event_Task_t Event_Task, Task_Destructor_t Destructor);

/**
 * @brief 注销任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Logout(Event_Task_Handle_t *task_handle);

/**
 * @brief 启动任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Start(Event_Task_Handle_t *task_handle);

/**
 * @brief 暂停任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Pause(Event_Task_Handle_t *task_handle);

#if USE_EVENT_DRIVEN
/**
 * @brief 向事件驱动操作系统注册事件
 * @param[in] task_handle 任务句柄
 * @param[in] event_id 事件id
 */
void Event_OS_Event_Register(Event_Task_Handle_t *task_handle, uint16_t event_id);

/**
 * @brief 向事件驱动操作系统注销事件
 * @param[in] task_handle 任务句柄
 * @param[in] event_id 事件id
 */
void Event_OS_Event_Logout(Event_Task_Handle_t *task_handle, uint16_t event_id);

/**
 * @brief 获取任务事件
 * @param[in] task_handle 任务句柄
 */
uint16_t Event_OS_Sub_Event(Event_Task_Handle_t *task_handle);

#if USE_HEAP
/**
 * @brief 获取任务事件数据
 * @param[in] task_handle 任务句柄
 * @param[out] data 接收数据指针
 */
uint16_t Event_OS_Sub_Data(Event_Task_Handle_t *task_handle,void **data);
#endif

/**
 * @brief 广播任务事件
 * @param[in] task_handle 任务句柄
 */
void Event_OS_Pub_Broadcast_Event(uint16_t event_id, void *data, uint16_t longth);

/**
 * @brief 发送任务事件
 * @param[in] task_handle 任务句柄
 */
void Event_OS_Pub_Event(uint16_t event_id, void *data, uint16_t longth);
#endif
/**
 * @} 任务操作函数
 */
/**
 * @brief 操作系统API
 * @{
 */
/**
 * @brief 事件操作系统时间戳函数
 */
#define Event_OS_GetTick() SEML_GetTick()
/**
 * @brief 事件操作系统延时函数
 */
#define Event_OS_Delay(ms) SEML_Delay(ms)

/**
 * @brief 事件操作系统初始化
 */
void Event_OS_Init(void);

/**
 * @brief 启动事件驱动操作系统
 */
void Event_OS_Run(void);

/**
 * @brief 事件操作系统回调函数
 * @param[in] config 回调函数配置
 */
void Event_OS_Callback(void *config);
#ifdef USE_CPU_UTILIZATION
/**
 * @brief 获取cpu使用率
 * @return cpu使用率(0-100)
*/
uint8_t Evnet_OS_Get_CPU_Utilization(void);
#endif
/**
 * @} 操作系统API
 */
#endif

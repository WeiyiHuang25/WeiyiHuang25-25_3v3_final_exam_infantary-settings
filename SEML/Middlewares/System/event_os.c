#include "event_os.h"

#if USE_EVENT_DRIVEN
/**
 * @brief 事件订阅表
 */
typedef struct
{
	Event_Task_Handle_t *List[EVENT_SUB_MAX_NUM]; /**< 订阅者句柄 */
	uint16_t num;																	/**< 订阅者数量 */
} Event_Sub_t;
#endif
/**
 * @brief 任务管理结构体
 */
typedef struct
{
	Event_Task_Handle_t *task_list; /**< 任务列表 */
	struct
	{
		uint16_t running : 1; /**< 正在运行任务管理 */
	} flag;
#if USE_EVENT_DRIVEN
	struct
	{
		s_queue queue;								 /**< 事件队列 */
		Event_t buffer[EVENT_MAX_NUM]; /**< 事件缓存 */
	} event;
#endif
} Task_Manage_t;

typedef struct
{
	Task_Manage_t high_priority; /**< 高优先级事件管理 */
	Task_Manage_t low_priority;	 /**< 低优先级事件管理 */
	struct
	{
		uint16_t used_callback : 1; /**< 使用回调函数 */
	} flag;
#if USE_EVENT_DRIVEN
	Event_Sub_t event_sub[EVENT_MAX_NUM]; /**< 事件订阅表 */
#endif
#if USE_CPU_UTILIZATION
	int8_t cpu_utilization;
	struct
	{
		uint32_t count;
		uint32_t base_count;
		uint32_t single_count;
	} cpu_utilization_config;
#endif
} Event_OS_t;

#if USE_HEAP
uint8_t heap_buffer[HEAP_SIZE];
#endif

Event_OS_t event_os;
/**
 * @brief 向任务列表添加任务
 * @param[out] task_manage 任务管理句柄
 * @param[in] task_handle 任务句柄
 */
static void Task_List_Add(Task_Manage_t *task_manage, Event_Task_Handle_t *task_handle)
{
	Event_Task_Handle_t *task_temp = task_manage->task_list;
	if (task_manage->task_list == NULL)
	{
		task_manage->task_list = task_handle;
		return;
	}
	else
	{
		while (task_temp->next != NULL)
			task_temp = task_temp->next;
		task_temp->next = task_handle;
	}
}
/**
 * @brief 从任务列表删除任务
 * @param[out] task_manage 任务管理句柄
 * @param[in] task_handle 任务句柄
 */
static void Task_List_Del(Task_Manage_t *task_manage, Event_Task_Handle_t *task_handle)
{
	Event_Task_Handle_t *task_temp = task_manage->task_list;
	// 从任务链表中删除
	if (task_manage->task_list == task_handle)
		task_manage->task_list = NULL;
	else
		while (task_handle->next != NULL)
			if (task_temp->next == task_handle)
			{
				task_temp->next = task_handle->next;
				break;
			}
}

/**
 * @brief 启动任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Start(Event_Task_Handle_t *task_handle)
{
	assert_param(task_handle != NULL);
	// 任务第一次启动执行构造函数
	if (task_handle->flag.initialized == 0)
	{
		if (task_handle->callback.Constructor != NULL)
			task_handle->callback.Constructor();
		task_handle->flag.initialized = 1;
	}
	task_handle->flag.run_status = EOS_Run;
}

/**
 * @brief 暂停任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Pause(Event_Task_Handle_t *task_handle)
{
	assert_param(task_handle != NULL);
	task_handle->flag.run_status = EOS_Stop;
}

/**
 * @brief 注册任务
 * @param[out] task_handle 任务句柄
 * @param[in] loop_delay 循环延时,该传入0不使能定时器
 * @param[in] priority 任务优先级
 * @param[in] Constructor 任务构造函数
 * @param[in] Event_Task 任务回调函数
 * @param[in] Destructor 任务析构函数
 */
void Event_OS_Task_Register(Event_Task_Handle_t *task_handle, uint32_t loop_delay, Event_OS_Priority_t priority, Task_Constructor_t Constructor, Event_Task_t Event_Task, Task_Destructor_t Destructor)
{
	assert_param(task_handle != NULL);
	assert_param(Event_Task != NULL);

	task_handle->callback.Constructor = Constructor;
	task_handle->callback.Destructor = Destructor;
	task_handle->callback.Event_Task = (void(*)(void *))Event_Task;

	task_handle->next = NULL;

	task_handle->timer.timeout_buckup = loop_delay;
	task_handle->timer.timeout_tick = 0;

	task_handle->flag.run_status = EOS_Stop;
	task_handle->flag.priority = priority;
	task_handle->flag.initialized = 0;
#if USE_EVENT_DRIVEN
#if USE_HEAP
	task_handle->event.data = 0;
	task_handle->event.longth = 0;
#endif
	task_handle->event.id = 0;
#endif
	if (priority == EOS_High_Priority)
		Task_List_Add(&event_os.high_priority, task_handle);
	else
		Task_List_Add(&event_os.low_priority, task_handle);
}

/**
 * @brief 注销任务
 * @param[in,out] task_handle 任务句柄
 */
void Event_OS_Task_Logout(Event_Task_Handle_t *task_handle)
{
	assert_param(task_handle != NULL);
	task_handle->flag.task_logout = 1;
	// 从任务列表中删除
	if (task_handle->flag.priority == EOS_High_Priority)
	{
		if (event_os.high_priority.flag.running == 1)
			return;
		Task_List_Del(&event_os.high_priority, task_handle);
	}
	else
	{
		if (event_os.low_priority.flag.running == 1)
			return;
		Task_List_Del(&event_os.low_priority, task_handle);
	}
	// 调用任务析构函数
	if (task_handle->callback.Destructor != NULL)
		task_handle->callback.Destructor();
	// 反初始化任务句柄
	task_handle->callback.Destructor = NULL;
	task_handle->callback.Event_Task = NULL;
	task_handle->next = NULL;

	task_handle->timer.timeout_buckup = 0;
	task_handle->timer.timeout_tick = 0;

	task_handle->flag.priority = EOS_Low_Priority;
	task_handle->flag.run_status = EOS_Stop;
	task_handle->flag.initialized = 0;
	task_handle->flag.task_logout = 0;

#if USE_EVENT_DRIVEN
#if USE_HEAP
	task_handle->event.data = 0;
	task_handle->event.longth = 0;
#endif
	task_handle->event.id = 0;
	for (int i = 0; i < EVENT_MAX_NUM; i++)
		Event_OS_Event_Logout(task_handle, i);
#endif
}

/**
 * @brief 任务循环
 */
static void Event_OS_Task_Loop(Task_Manage_t *task_manage, Event_OS_Priority_t priority)
{
	Event_Task_Handle_t *temp_task, *temp1_task;
	task_manage->flag.running = 1;
	temp_task = task_manage->task_list;
#if USE_EVENT_DRIVEN
	Event_t event;
	// 遍历事件订阅表
	while (1)
	{
		if (DeQueue(&task_manage->event.queue, &event) == SEML_OK)
		{
			if (event.flag.broadcast == 1)
			{
				temp_task = task_manage->task_list;
				while (temp_task != NULL)
				{
					temp_task->event = event;
					temp_task->callback.Event_Task(temp_task);
					temp_task = temp_task->next;
				}
			}
			else
				for (int i = 0; i < event_os.event_sub[event.id].num; i++)
				{
					event_os.event_sub[event.id].List[i]->event = event;
					event_os.event_sub[event.id].List[i]->callback.Event_Task(event_os.event_sub[event.id].List[i]);
				}
#if USE_HEAP
			free(event.data);
			event.data = NULL;
#endif
		}
		else
			break;
	}
#if USE_HEAP
	event.data = NULL;
	event.longth = 0;
#endif
	event.flag.broadcast = 0;
	event.id = 0xffff;
#endif // USE_EVENT_DRIVEN
	temp_task = task_manage->task_list;

	while (temp_task != NULL)
	{
		// 定时任务循环
		if (temp_task->flag.run_status == EOS_Run && temp_task->timer.timeout_buckup != 0)
			if (Event_OS_GetTick() >= temp_task->timer.timeout_tick)
			{
				// 重置定时时间
				temp_task->timer.timeout_tick = Event_OS_GetTick() + temp_task->timer.timeout_buckup;
#if USE_EVENT_DRIVEN
				temp_task->event = event;
#endif
				// 检查任务是否需要注销
				if (temp_task->flag.task_logout == 1)
				{
					temp1_task = temp_task->next;
					task_manage->flag.running = 0;
					Event_OS_Task_Logout(temp_task);
					task_manage->flag.running = 1;
					temp_task = temp1_task;
				}
				else
					temp_task->callback.Event_Task(temp_task);
			}
		temp_task = temp_task->next;
	}
	task_manage->flag.running = 0;
}

/**
 * @brief 事件操作系统回调函数
 * @param[in] config 回调函数配置
 */
void Event_OS_Callback(void *config)
{
	event_os.flag.used_callback = 1;
	Event_OS_Task_Loop(&event_os.high_priority, EOS_High_Priority);
}

#if USE_CPU_UTILIZATION
#define CPU_UTILIZATION_SAMPLE_TIME 50
/**
 * @brief 标定cpu使用率
 */
void Event_OS_Calibration_CPU_Utilization(void)
{
	uint32_t start_tick = SEML_GetTick();
	const uint32_t sample_time = 100;
	event_os.cpu_utilization_config.base_count = 0;
	event_os.cpu_utilization_config.count = 0;
	// 在没有运行任务的时候统计单位时间计数值
	while (1)
	{
		if (SEML_GetTick() - start_tick > sample_time)
			break;
		else
			event_os.cpu_utilization_config.count++;
		Event_OS_Task_Loop(&event_os.high_priority, EOS_High_Priority);
		Event_OS_Task_Loop(&event_os.low_priority, EOS_Low_Priority);
	}
	event_os.cpu_utilization_config.base_count = event_os.cpu_utilization_config.count * CPU_UTILIZATION_SAMPLE_TIME / 100;
	// 在没有运行任务的时候统计单位时间计数值
	event_os.cpu_utilization_config.count = 0;
	start_tick = SEML_GetTick();
	while (1)
	{
		if (SEML_GetTick() - start_tick > sample_time)
			break;
		else
			event_os.cpu_utilization_config.count++;
		Event_OS_Task_Loop(&event_os.low_priority, EOS_Low_Priority);
	}
	event_os.cpu_utilization_config.single_count = event_os.cpu_utilization_config.count * CPU_UTILIZATION_SAMPLE_TIME / 100;
	event_os.cpu_utilization_config.count = 0;
}

/**
 * @brief 获取cpu使用率
 * @return cpu使用率(0-100)
 */
uint8_t Evnet_OS_Get_CPU_Utilization(void)
{
	return event_os.cpu_utilization;
}

#endif

/**
 * @brief 事件操作系统初始化
 */
void Event_OS_Init(void)
{
#if USE_CPU_UTILIZATION
	Event_OS_Calibration_CPU_Utilization();
#endif
#if USE_EVENT_DRIVEN
	InitQueue(&event_os.high_priority.event.queue, sizeof(Event_t), EVENT_MAX_NUM, &event_os.high_priority.event.buffer, queue_full_hander_error);
	InitQueue(&event_os.low_priority.event.queue, sizeof(Event_t), EVENT_MAX_NUM, &event_os.low_priority.event.buffer, queue_full_hander_error);
#endif
#if USE_HEAP
	mem_init((void *)heap_buffer, HEAP_SIZE);
#endif
}

/**
 * @brief 启动事件驱动操作系统
 */
void Event_OS_Run(void)
{
	Event_Task_Handle_t *temp_task = event_os.low_priority.task_list;
	static uint32_t cpu_update_start = 0;
	// 检查是否存在时基
	assert_param(Event_OS_GetTick() != 0);
	// 检查是否标定了cpu使用率
	if (event_os.cpu_utilization_config.base_count == 0)
		Event_OS_Calibration_CPU_Utilization();
	// 开启所有任务
	while (temp_task != NULL)
	{
		assert_param(temp_task->callback.Event_Task != NULL);
		Event_OS_Task_Start(temp_task);
		temp_task = temp_task->next;
	}
	temp_task = event_os.high_priority.task_list;
	while (temp_task != NULL)
	{
		assert_param(temp_task->callback.Event_Task != NULL);
		Event_OS_Task_Start(temp_task);
		temp_task = temp_task->next;
	}
	Event_OS_Delay(100);
	// 检查是否使用了回调函数
	if (event_os.flag.used_callback != 1)
		while (1)
		{
#if USE_CPU_UTILIZATION
			// 更新cpu使用率
			if (SEML_GetTick() - cpu_update_start > CPU_UTILIZATION_SAMPLE_TIME)
			{
				event_os.cpu_utilization = 100 - event_os.cpu_utilization_config.count * 100 / event_os.cpu_utilization_config.base_count;
				event_os.cpu_utilization_config.count = 0;
				cpu_update_start = SEML_GetTick();
			}
			else
				event_os.cpu_utilization_config.count++;
#endif
			Event_OS_Task_Loop(&event_os.high_priority, EOS_High_Priority);
			Event_OS_Task_Loop(&event_os.low_priority, EOS_Low_Priority);
		}
	else
	{
		event_os.cpu_utilization_config.base_count = event_os.cpu_utilization_config.single_count;
		while (1)
		{
#if USE_CPU_UTILIZATION
			// 更新cpu使用率
			if (SEML_GetTick() - cpu_update_start > CPU_UTILIZATION_SAMPLE_TIME)
			{
				event_os.cpu_utilization = 100 - event_os.cpu_utilization_config.count * 100 / event_os.cpu_utilization_config.base_count;
				event_os.cpu_utilization_config.count = 0;
				cpu_update_start = SEML_GetTick();
			}
			else
				event_os.cpu_utilization_config.count++;
#endif
			Event_OS_Task_Loop(&event_os.low_priority, EOS_Low_Priority);
		}
	}
}

#if USE_EVENT_DRIVEN
/**
 * @brief 向事件驱动操作系统注册事件
 * @param[in] task_handle 任务句柄
 * @param[in] event_id 事件id
 */
void Event_OS_Event_Register(Event_Task_Handle_t *task_handle, uint16_t event_id)
{
	for (int i = 0; i < event_os.event_sub[event_id].num; i++)
		if (event_os.event_sub[event_id].List[i] == task_handle)
			return;
	event_os.event_sub[event_id].List[event_os.event_sub[event_id].num] = task_handle;
	event_os.event_sub[event_id].num++;
}

/**
 * @brief 向事件驱动操作系统注销事件
 * @param[in] task_handle 任务句柄
 * @param[in] event_id 事件id
 */
void Event_OS_Event_Logout(Event_Task_Handle_t *task_handle, uint16_t event_id)
{
	for (int i = 0; i < event_os.event_sub[event_id].num; i++)
		if (event_os.event_sub[event_id].List[i] == task_handle)
		{
			event_os.event_sub[event_id].List[i] = event_os.event_sub[event_id].List[event_os.event_sub[event_id].num - 1];
			event_os.event_sub[event_id].num--;
			return;
		}
}

/**
 * @brief 获取任务事件
 * @param[in] task_handle 任务句柄
 */
uint16_t Event_OS_Sub_Event(Event_Task_Handle_t *task_handle)
{
	return task_handle->event.id;
}

#if USE_HEAP
/**
 * @brief 获取任务事件数据
 * @param[in] task_handle 任务句柄
 * @param[out] data 接收数据指针
 */
uint16_t Event_OS_Sub_Data(Event_Task_Handle_t *task_handle, void **data)
{
	*data = task_handle->event.data;
	return task_handle->event.longth;
}
#endif

/**
 * @brief 发送任务事件
 * @param[in] task_handle 任务句柄
 */
void Event_OS_Pub_Event(uint16_t event_id, void *data, uint16_t longth)
{
	Event_t event;
	event.id = event_id;
	event.flag.broadcast = 0;
#if USE_HEAP
	event.data = mem_malloc(longth);
	if (event.data != NULL)
	{
		event.longth = longth;
		mem_copy(event.data, data, longth);
	}
#endif
	EnQueue(&event_os.high_priority.event.queue, &event);
	EnQueue(&event_os.low_priority.event.queue, &event);
}

/**
 * @brief 广播任务事件
 * @param[in] task_handle 任务句柄
 */
void Event_OS_Pub_Broadcast_Event(uint16_t event_id, void *data, uint16_t longth)
{
	Event_t event;
	event.id = event_id;
	event.flag.broadcast = 1;
#if USE_HEAP
	event.data = mem_malloc(longth);
	if (event.data != NULL)
	{
		event.longth = longth;
		mem_copy(event.data, data, longth);
	}
#endif
	EnQueue(&event_os.high_priority.event.queue, &event);
	EnQueue(&event_os.low_priority.event.queue, &event);
}
#endif

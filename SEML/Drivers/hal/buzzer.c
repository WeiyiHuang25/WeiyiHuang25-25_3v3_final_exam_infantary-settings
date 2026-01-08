#include "buzzer.h"
Buzzer_t *buzzer_list;

/**
 * @brief 初始化蜂鸣器
 * @param buzzer 蜂鸣器句柄
 * @param pwm_handle pwm句柄
 */
void Buzzer_Init(Buzzer_t *buzzer, PWM_IF_t *htim,uint32_t channel,uint32_t timer_clock_frec)
{
	Buzzer_t *temp_handle;
	buzzer->next = 0;
	buzzer->timeout = 0;
	buzzer->task_num = 0;
	InitQueue(&buzzer->task_queue, sizeof(Buzzer_Task_t), BUZZER_TASK_NUM-1, buzzer->task_buffer, queue_full_hander_error);
	if (buzzer_list == NULL)
		buzzer_list = buzzer;
	else
	{
		temp_handle = buzzer_list;
		while (temp_handle->next != NULL)
			temp_handle = temp_handle->next;
		temp_handle->next = buzzer;
	}
#if USE_HAL_TIM_REGISTER_CALLBACKS == 1
	HAL_TIM_RegisterCallback(htim,HAL_TIM_PERIOD_ELAPSED_CB_ID,(pTIM_CallbackTypeDef)Buzzer_Callback);
#endif
	SEML_PWM_Init(&buzzer->pwm_handle,htim,channel,timer_clock_frec);
	SEML_PWM_Start(&buzzer->pwm_handle);

}

/**
 * 
 * @brief 蜂鸣器任务
 * @param config 蜂鸣器句柄
 */
void Buzzer_Handle(Buzzer_t *buzzer)
{
	Buzzer_Task_t buzzer_task;
	if (SEML_GetTick() > buzzer->timeout)
	{
		if (DeQueue(&buzzer->task_queue, &buzzer_task) != SEML_OK)
		{
			SEML_PWM_Set_Duty(&buzzer->pwm_handle, 0);
			return;
		}
		buzzer->task_num--;
		buzzer->timeout = buzzer_task.time + SEML_GetTick();
		SEML_PWM_Set_Frec(&buzzer->pwm_handle, buzzer_task.frec);
		SEML_PWM_Set_Duty(&buzzer->pwm_handle, buzzer_task.volume * 0.5f / 100.0f);
	}
}

/**
 * @brief 蜂鸣器回调函数
 * @param config 定时器句柄
 */
void Buzzer_Callback(void *config)
{
	Buzzer_t *buzzer = buzzer_list;
	while (buzzer != NULL)
	{
		if (buzzer->pwm_handle.timer_handle == config)
			break;
		if (buzzer->next == NULL)
			return;
	}
	Buzzer_Handle(buzzer);
}

/**
 * @brief 蜂鸣器添加任务
 * @param buzzer 蜂鸣器句柄
 * @param frec 频率(Hz)
 * @param volume 音量(0-100)
 * @param time 时间(ms)
 * @return 添加成功返回SEML_OK
 */
SEML_StatusTypeDef Buzzer_Add_Task(Buzzer_t *buzzer, uint32_t frec, uint8_t volume, uint16_t time)
{
	Buzzer_Task_t buzzer_task;
	buzzer_task.frec = frec;
	buzzer_task.time = time;
	buzzer_task.volume = volume;
	if (EnQueue(&buzzer->task_queue, &buzzer_task) == SEML_OK)
	{
		buzzer->task_num++;
		return SEML_OK;
	}
	return SEML_ERROR;
}

/**
 * @brief 蜂鸣器任务--哔三声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef 
 */
SEML_StatusTypeDef Buzzer_Task_B_B_B(Buzzer_t *buzzer, uint8_t volume)
{
	if (Buzzer_Available_Tasks_Num(buzzer) <= 6)
		return SEML_ERROR;
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	return SEML_OK;
}

/**
 * @brief 蜂鸣器任务--哔两声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef 
 */
SEML_StatusTypeDef Buzzer_Task_B_B(Buzzer_t *buzzer, uint8_t volume)
{
	if (Buzzer_Available_Tasks_Num(buzzer) <= 4)
		return SEML_ERROR;
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	return SEML_OK;
}

/**
 * @brief 蜂鸣器任务--哔一声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef 
 */
SEML_StatusTypeDef Buzzer_Task_B(Buzzer_t *buzzer, uint8_t volume)
{
	if (Buzzer_Available_Tasks_Num(buzzer) <= 2)
		return SEML_ERROR;
	Buzzer_Add_Task(buzzer, 6000, volume, 100);
	Buzzer_Add_Task(buzzer, 6000, 0, 100);
	return SEML_OK;
}

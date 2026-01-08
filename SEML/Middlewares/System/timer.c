#include "timer.h"
Soft_Timer_t *soft_timer;

/**
 * @brief 软件定时器初始化
 * @param[out] hstim 软件定时器句柄
 * @param[in] mode 定时器模式
 * @param[in] timeout 定时时间(以调用的硬件定时器为时基的整数倍)
 * @param[in] callback_fun 回调函数
 * @param[out] callback_config 回调函数配置
 */
void SEML_Timer_Create(Soft_Timer_t *hstim, Timer_AutoReload mode, uint32_t timeout, void (*callback_fun)(void *config), void *callback_config)
{
	Soft_Timer_t *temp_hstim;
	assert_param(hstim != NULL);
	assert_param(callback_fun != NULL);
	assert_param(IS_TIMER_AUTORELOAD(mode));

	hstim->callback_fun = callback_fun;
	hstim->callback_config = callback_config;
	hstim->mode = mode;
	hstim->status = Timer_Run;
	hstim->timeout_buckup = timeout;
	hstim->timeout_count = timeout;
	hstim->next = NULL;
	if (soft_timer == NULL)
		soft_timer = hstim;
	else
	{
		// 遍历是否存在于链表中
		temp_hstim = soft_timer;
		while (temp_hstim->next != NULL)
		{
			if (temp_hstim == hstim)
				return;
			temp_hstim = temp_hstim->next;
		}
		// 不存在开始添加
		temp_hstim = soft_timer;
		while (temp_hstim->next != NULL)
			temp_hstim = temp_hstim->next;
		temp_hstim->next = hstim;
	}
}

/**
 * @brief 删除软件定时器
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Delete(Soft_Timer_t *hstim)
{
	Soft_Timer_t *temp_hstim = soft_timer;
	assert_param(hstim != NULL);
	if (hstim == soft_timer)
		soft_timer = soft_timer->next;
	else
		while (temp_hstim->next != NULL)
		{
			if (temp_hstim->next == hstim)
			{
				temp_hstim->next = ((Soft_Timer_t *)temp_hstim->next)->next;
				break;
			}
			temp_hstim = temp_hstim->next;
		}
	hstim->callback_fun = NULL;
	hstim->callback_config = NULL;
	hstim->mode = Timer_Single;
	hstim->status = Timer_Stop;
	hstim->timeout_buckup = 0;
	hstim->timeout_count = 0;
	hstim->next = NULL;
}

/**
 * @brief 软件定时器复位
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Reset(Soft_Timer_t *hstim)
{
	assert_param(hstim != NULL);
	hstim->timeout_count = hstim->timeout_buckup;
}

/**
 * @brief 软件定时器启动
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Start(Soft_Timer_t *hstim)
{
	assert_param(hstim != NULL);
	hstim->status = Timer_Run;
}

/**
 * @brief 软件定时器关闭
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Stop(Soft_Timer_t *hstim)
{
	assert_param(hstim != NULL);
	hstim->status = Timer_Stop;
}

/**
 * @brief 软件定时器回调函数
 * @param[in] config 回调函数配置
 */
void SEML_Timer_Callback(void *config)
{
	Soft_Timer_t *temp_hstim = soft_timer;
	void (*callback_fun)(void *config);
	void *callback_config;
	while (temp_hstim != NULL)
	{
		if (temp_hstim->status == Timer_Run)
			if (--temp_hstim->timeout_count == 0)
			{
				callback_fun = temp_hstim->callback_fun;
				callback_config = temp_hstim->callback_config;
				// 循环计数的重置计数器
				if (temp_hstim->mode == Timer_Cycle)
					temp_hstim->timeout_count = temp_hstim->timeout_buckup;
				// 单次计数的删除软件定时器
				else
					SEML_Timer_Delete(temp_hstim);
				// 超时回调
				if (callback_fun != NULL)
					callback_fun(callback_config);
			}
		temp_hstim = temp_hstim->next;
	}
}

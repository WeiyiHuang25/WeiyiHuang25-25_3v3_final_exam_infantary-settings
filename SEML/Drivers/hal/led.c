#include "led.h"
LED_t *led_root;

#define LED_Gamma_Transform(light) ((light) * (light))

/**
 * @brief LED初始化(pwm模式)
 * 
 * @param[in,out] led led句柄
 * @param[in] htim 定时器句柄
 * @param[in] channel led所在的定时器通道
 * @param[in] timer_clock_frec 定时器时基频率
 */
void LED_Init_PWM(LED_t *led, PWM_IF_t *htim, uint32_t channel, uint32_t timer_clock_frec)
{
	LED_t *led_temp = led_root;
	assert_param(led != NULL);
	assert_param(htim != NULL);
	led->period = 1000;
	led->light = 1;
	led->duty = 32768;
	led->phase = 0;
	led->mode = LED_OFF_Mode;

	led->next = NULL;
	if (led_root != NULL)
	{
		while (led_temp->next != NULL)
		{
			// 链表中查找到不再添加
			if (led_temp == led)
				break;
			led_temp = (LED_t *)led_temp->next;
		}
		led_temp->next = (void *)led;
	}
	else
	{
		led_root = led;
#if USE_HAL_TIM_REGISTER_CALLBACKS == 1
		HAL_TIM_RegisterCallback(htim, HAL_TIM_PERIOD_ELAPSED_CB_ID, (pTIM_CallbackTypeDef)LED_Handle_Callback);
#endif
	}
	// 先添加回调后才能开启定时器
	SEML_PWM_Init(&led->pwm, htim, channel, timer_clock_frec);
	// 100Hz人眼看不出闪烁
	SEML_PWM_Set_Frec(&led->pwm, 1000);
	SEML_PWM_Set_Duty(&led->pwm, 0);
	SEML_PWM_Start(&led->pwm);
}

/**
 * @brief 设置LED工作模式
 * 
 * @param[in,out] led led句柄
 * @param[in] mode 工作模式,参数枚举位于LED_Mode_t
 */
void LED_Set_Mode(LED_t *led, LED_Mode_t mode)
{
	assert_param(led != NULL);
	assert_param(IS_LED_MODE(mode));
	led->mode = mode;
}

/**
 * @brief 设置LED闪烁(呼吸)周期
 * 
 * @param[in,out] led led句柄
 * @param[in] period led一个周期时间(毫秒,ms),默认1000
 */
void LED_Set_Period(LED_t *led, uint16_t period)
{
	assert_param(led != NULL);
	led->period = period;
}

/**
 * @brief 设置LED闪烁占空比
 * 
 * @param[in,out] led led句柄
 * @param[in] duty led闪烁占空比(0~1),默认0.5f
 */
void LED_Set_Duty(LED_t *led, float duty)
{
	assert_param(led != NULL);
	led->duty = duty * 65535;
}

/**
 * @brief 设置LED亮度
 * 
 * @param[in,out] led led句柄
 * @param[in] duty led亮度占比(0~1),默认1
 */
void LED_Set_Light(LED_t *led, float light)
{
	assert_param(led != NULL);
	led->light = light;
}

/**
 * @brief 设置LED相位
 * 
 * @param[in,out] led led句柄
 * @param[in] duty led相位偏移时间(毫秒,ms),默认0
 */
void LED_Set_Phase(LED_t *led, uint16_t phase)
{
	assert_param(led != NULL);
	led->phase = phase;
}

/**
 * @brief LED中断服务函数
 * 
 * @param[in,out] led led句柄
 */
static void LED_Handle(LED_t *led)
{
	uint32_t temp;
	float led_light;
	assert_param(led != NULL);
	switch (led->mode)
	{
	case LED_OFF_Mode:
		SEML_PWM_Set_Duty(&led->pwm, 0);
		break;
	case LED_Light_Mode:
		SEML_PWM_Set_Duty(&led->pwm, LED_Gamma_Transform(led->light));
		break;
	case LED_Breath_Mode:
		temp = (SEML_GetTick() + led->phase) % led->period;
		if (temp > led->period / 2)
			temp = led->period - temp;
		led_light = 2 * temp / (float)led->period * led->light;
		// gamma:2		
			SEML_PWM_Set_Duty(&led->pwm, LED_Gamma_Transform(led_light));
		break;
	case LED_Twinkle_Mode:
		temp = (SEML_GetTick() + led->phase) % led->period;
		if (temp < ((uint32_t)led->duty * led->period) / 65536)
			SEML_PWM_Set_Duty(&led->pwm, LED_Gamma_Transform(led->light));
		else
			SEML_PWM_Set_Duty(&led->pwm, 0);
		break;
	default:
		assert_param(0);
	}
}

/**
 * @brief LED中断服务回调函数
 * 放入任意一个1ms定时器中即可,若开启注册回调可以不管
 * @param[in] config 回调函数配置
 */
void LED_Handle_Callback(void *config)
{
	LED_t *led_temp = led_root;
	if (led_temp != NULL)
	{
		while (led_temp)
		{
			LED_Handle(led_temp);
			led_temp = (LED_t *)led_temp->next;
		}
	}
}

#include "WS281x.h"
#include <string.h>

#ifndef PIXEL_NUM
#define PIXEL_NUM 1000
#endif

#define WS_ZERO 5
#define WS_ONE 25

WS281x_PWM_Driver_t *WS281x_pwm_driver;

/**
 * @brief WS281x PWM驱动模式初始化
 * @param handle WS281xPWM驱动句柄
 * @param pixel_buffer 像素点缓存数组 需注意申请大小需要乘以GRB(24)
 * @param pixel_num 像素点个数
 * @param htim 定时器句柄
 * @param tim_channel 定时器通道
 * @param period 自动重装载计数器值
 * @return WS281x句柄
 */
WS281x_t *WS281x_PWM_Driver_Init(WS281x_PWM_Driver_t *handle,
																 uint16_t *pixel_buffer, uint16_t pixel_num,
																 void *htim, uint32_t tim_channel, uint32_t period)
{
	WS281x_PWM_Driver_t *pwm_driver = WS281x_pwm_driver;

	assert_param(handle != NULL);
	assert_param(pixel_buffer != NULL);
	assert_param(htim != NULL);
	assert_param(pixel_num != 0);
	assert_param(period != 0);

	// T0H:350ns T0L:800ns T1H:700ns T1L:600ns err:±150ns RST:50us
	handle->one_count = period * 2 / 3;
	handle->zero_count = period / 3;

	handle->pixel_buffer = pixel_buffer;
	handle->pixel_num = pixel_num;
	for (uint32_t i = 0; i < pixel_num * GRB; i++)
		pixel_buffer[i] = 0;

	handle->htim = htim;
	handle->tim_channel = tim_channel;

	handle->WS281x.Driver = handle;
	handle->WS281x.Driver_mode = WS281x_PWM_Driver;

	handle->next = NULL;
	if (pwm_driver != NULL)
	{
		while (pwm_driver->next != NULL)
			pwm_driver = pwm_driver->next;
		pwm_driver->next = handle;
	}
	else
		WS281x_pwm_driver = handle;

	return &handle->WS281x;
}
/**
 * @brief WS281x显示函数
 * @param handle WS281x句柄
 */
void WS281x_Show(WS281x_t *handle)
{
	assert_param(handle != NULL);
	assert_param(handle->Driver != NULL);

	switch (handle->Driver_mode)
	{
	case WS281x_PWM_Driver:
		WS281x_PWM_Driver_t *pwm_driver = (WS281x_PWM_Driver_t *)handle->Driver;
		HAL_TIM_PWM_Start_DMA(pwm_driver->htim, pwm_driver->tim_channel, (uint32_t *)pwm_driver->pixel_buffer, pwm_driver->pixel_num * GRB);
		break;
	default:
		assert_failed(__FILE__, __LINE__);
	}
}

/**
 * @brief 将rgb通道颜色合并成ws281x报文格式
 * @param r 红
 * @param g 绿
 * @param b 蓝
 * @return uint32_t
 */
uint32_t WS281x_Color(uint8_t r, uint8_t g, uint8_t b)
{
	return (uint32_t)(g << 16 | r << 8 | b);
}

/**
 * @brief WS281x设置第n个点的颜色
 * @param handle WS281x句柄
 * @param n 位置
 * @param color 颜色
 */
void WS281x_SetColor(WS281x_t *handle, uint16_t n, uint32_t color)
{
	uint16_t *temp;
	assert_param(handle != NULL);
	assert_param(handle->Driver != NULL);
	switch (handle->Driver_mode)
	{
	case WS281x_PWM_Driver:
		WS281x_PWM_Driver_t *pwm_driver = (WS281x_PWM_Driver_t *)handle->Driver;
		if (n < pwm_driver->pixel_num)
		{
			temp = pwm_driver->pixel_buffer + n * GRB;
			for (uint32_t i = 0; i < GRB; ++i)
				temp[i] = ((color << i) & 0x800000) ? pwm_driver->one_count : pwm_driver->zero_count;
		}
		break;
	default:
		assert_failed(__FILE__, __LINE__);
	}
}

/**
 * @brief 将ws281x熄灭，清空缓冲区
 * @param handle WS281x句柄
 */
void WS281x_Close(WS281x_t *handle)
{
	assert_param(handle != NULL);
	assert_param(handle->Driver != NULL);
	switch (handle->Driver_mode)
	{
	case WS281x_PWM_Driver:
		WS281x_PWM_Driver_t *pwm_driver = (WS281x_PWM_Driver_t *)handle->Driver;
		for (uint32_t i = 0; i < pwm_driver->pixel_num * GRB; i++)
			pwm_driver->pixel_buffer[i] = pwm_driver->zero_count;
		break;
	default:
		assert_failed(__FILE__, __LINE__);
	}
	WS281x_Show(handle);
}

/**
 * @brief WS281x定时器回调函数
 * @param htim 定时器句柄
 */
void WS281x_TIMCallback(TIM_HandleTypeDef *htim)
{
	WS281x_PWM_Driver_t *pwm_driver = WS281x_pwm_driver;
	static uint32_t temp;
	// HAL_TIM_PWM_PULSE_FINISHED_CB_ID
	while (pwm_driver != NULL)
	{
		temp = pwm_driver->tim_channel >> 2;
		if ((uint32_t)pwm_driver->htim == (uint32_t)htim && 0x01 << temp == htim->Channel)
		{
			HAL_TIM_PWM_Stop_DMA(htim, pwm_driver->tim_channel);
			__HAL_TIM_SetCompare(htim, pwm_driver->tim_channel, 0); // 占空比清0，若不清会导致灯珠颜色不对
			return;
		}
		pwm_driver = pwm_driver->next;
	}
}

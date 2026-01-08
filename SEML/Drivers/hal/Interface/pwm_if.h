#ifndef _PWM_IF_H_
#define _PWM_IF_H_
#include "SEML_common.h"
#ifdef USE_HAL_DRIVER
typedef TIM_HandleTypeDef PWM_IF_t;
#else
#error Error: pwm interface is not configured(pwm_if.h)
#endif 
typedef struct 
{
	PWM_IF_t* timer_handle;
	uint32_t channel;
	uint32_t timer_clock_frec;
}PWM_Handle_t;

/**
 * @brief PWM初始化
 * @param pwm_handle PWM句柄
 * @param htim 定时器句柄 
 * @param channel 通道
 * @param timer_clock_frec 定时器时钟频率
 */
void SEML_PWM_Init(PWM_Handle_t *pwm_handle,PWM_IF_t *htim,uint32_t channel,uint32_t timer_clock_frec);

/**
 * @brief 设置PWM频率
 * @param pwm_handle PWM句柄
 * @param frec 设置频率
 */
void SEML_PWM_Set_Frec(PWM_Handle_t *pwm_handle,uint32_t frec);

/**
 * @brief 设置PWM占空比
 * @param pwm_handle PWM句柄
 * @param duty 占空比(0-1)
 */
void SEML_PWM_Set_Duty(PWM_Handle_t *pwm_handle,float duty);

/**
 * @brief 开启PWM输出
 * @param pwm_handle PWM句柄
 */
void SEML_PWM_Start(PWM_Handle_t *pwm_handle);

/**
 * @brief 关闭PWM输出
 * @param pwm_handle PWM句柄
 */
void SEML_PWM_Stop(PWM_Handle_t *pwm_handle);

#endif

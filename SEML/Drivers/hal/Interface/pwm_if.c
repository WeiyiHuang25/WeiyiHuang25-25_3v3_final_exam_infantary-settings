#include "pwm_if.h"
#if USE_HAL_DRIVER
#define __SEML_PWM_Start(pwm) HAL_TIM_PWM_Start((pwm)->timer_handle, (pwm)->channel)
#define __SEML_PWM_Stop(pwm) HAL_TIM_PWM_Stop((pwm)->timer_handle, (pwm)->channel)
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
#define __SEML_TIM_MSP_Init(pwm) HAL_TIM_Base_Start_IT((pwm)->timer_handle)
#else
#define __SEML_TIM_MSP_Init(pwm)
#endif
#else
#error Error: pwm interface is not configured(pwm_if.h)
#endif
#if USE_HAL_DRIVER
/**
 * @brief PWM初始化
 * @param pwm_handle PWM句柄
 * @param htim 定时器句柄 
 * @param channel 通道
 * @param timer_clock_frec 定时器时钟频率
 */
void SEML_PWM_Init(PWM_Handle_t *pwm_handle,PWM_IF_t *htim,uint32_t channel,uint32_t timer_clock_frec)
{
	assert_param(pwm_handle != NULL);
	assert_param(htim != NULL);
	assert_param(IS_TIM_CHANNELS(channel));
	pwm_handle->channel = channel;
	pwm_handle->timer_handle = htim;
	pwm_handle->timer_clock_frec = timer_clock_frec;
	__SEML_TIM_MSP_Init(pwm_handle);
}
#endif

/**
 * 
 * @brief 设置PWM频率
 * @param pwm_handle PWM句柄
 * @param frec 设置频率
 */
void SEML_PWM_Set_Frec(PWM_Handle_t *pwm_handle,uint32_t frec)
{
	assert_param(pwm_handle != NULL);
	assert_param(pwm_handle->timer_handle != NULL);
#if USE_HAL_DRIVER
	TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)pwm_handle->timer_handle;
	htim->Instance->PSC = pwm_handle->timer_clock_frec/((htim->Instance->ARR+1)*frec) - 1;
#endif
}

/**
 * @brief 设置PWM占空比
 * @param pwm_handle PWM句柄
 * @param duty 占空比(0-1)
 */
void SEML_PWM_Set_Duty(PWM_Handle_t *pwm_handle,float duty)
{
	uint32_t temp;
	assert_param(pwm_handle != NULL);
	assert_param(pwm_handle->timer_handle != NULL);
#if USE_HAL_DRIVER
	TIM_HandleTypeDef *htim = pwm_handle->timer_handle;
	temp = duty * htim->Instance->ARR;
	__HAL_TIM_SET_COMPARE(htim,pwm_handle->channel,temp);
#endif
}

/**
 * @brief 开启PWM输出
 * @param pwm_handle PWM句柄
 */
void SEML_PWM_Start(PWM_Handle_t *pwm_handle)
{
	__SEML_PWM_Start(pwm_handle);
}

/**
 * @brief 关闭PWM输出
 * @param pwm_handle PWM句柄
 */
void SEML_PWM_Stop(PWM_Handle_t *pwm_handle)
{
	__SEML_PWM_Stop(pwm_handle);
}

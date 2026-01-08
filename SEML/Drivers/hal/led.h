
/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : led.h
 * @author : SY7_yellow
 * @brief  : LED组件
 * @date   : 2023-10-16
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2024-2-6       <td>1.0         <td>SY7_yellow  <td>实现了基础功能
 * </table>
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 调用LED_Init传入led句柄,pwm配置
 * 2. 若没开启注册回调,需要将LED_Handle_Callback放入1ms中断中执行
 * 3. 使用LED_Set_Mode配置工作模式,共有下列几种工作模式：
 * 	   - LED_OFF_Mode:LED关闭
 *     - LED_Light_Mode:LED常亮模式
 *     - LED_Breath_Mode:LED呼吸模式
 *     - LED_Twinkle_Mode:LED闪烁模式
 * 4. LED工作有四个参数:周期,占空比,相位,亮度,可以调用对应操作函数进行设置
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 *
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
#ifndef _LED_H_
#define _LED_H_
#include "SEML_common.h"
#include "pwm_if.h"
#include "math_common.h"
/**
 * @brief LED工作模式
 */
typedef enum
{
	LED_OFF_Mode,			/**< LED关 */
	LED_Light_Mode,		/**< LED常亮模式 */
	LED_Breath_Mode,	/**< LED呼吸模式 */
	LED_Twinkle_Mode /**< LED闪烁模式 */
} LED_Mode_t;
#define IS_LED_MODE(param) ((param) >= LED_OFF_Mode && (param) <= LED_Twinkle_Mode)

/**
 * @brief LED句柄
 */
typedef struct
{
	PWM_Handle_t pwm; /**< pwm句柄 */
	uint16_t period;	/**< 周期 */
	uint16_t duty;		/**< 占空比 */
	uint16_t phase;		/**< 相位 */
	float light;			/**<  亮度 */
	LED_Mode_t mode;	/**< 工作模式 */
	void *next;
} LED_t;

/**
 * @brief LED初始化(pwm模式)
 *
 * @param[in,out] led led句柄
 * @param[in] htim 定时器句柄
 * @param[in] channel led所在的定时器通道
 * @param[in] timer_clock_frec 定时器时基频率
 */
void LED_Init_PWM(LED_t *led, PWM_IF_t *htim, uint32_t channel, uint32_t timer_clock_frec);

/**
 * @brief 设置LED工作模式
 *
 * @param[in,out] led led句柄
 * @param[in] mode 工作模式,参数枚举位于LED_Mode_t
 */
void LED_Set_Mode(LED_t *led, LED_Mode_t mode);

/**
 * @brief 设置LED闪烁(呼吸)周期
 *
 * @param[in,out] led led句柄
 * @param[in] period led一个周期时间(毫秒,ms),默认1000
 */
void LED_Set_Period(LED_t *led, uint16_t period);

/**
 * @brief 设置LED闪烁占空比
 *
 * @param[in,out] led led句柄
 * @param[in] duty led闪烁占空比(0~1),默认0.5f
 */
void LED_Set_Duty(LED_t *led, float duty);

/**
 * @brief 设置LED亮度
 *
 * @param[in,out] led led句柄
 * @param[in] duty led亮度占比(0~1),默认1
 */
void LED_Set_Light(LED_t *led, float light);

/**
 * @brief 设置LED相位
 *
 * @param[in,out] led led句柄
 * @param[in] duty led相位偏移时间(毫秒,ms),默认0
 */
void LED_Set_Phase(LED_t *led, uint16_t phase);

/**
 * @brief LED中断服务回调函数
 * 放入任意一个1ms定时器中即可,若开启注册回调可以不管
 * @param[in] config 回调函数配置
 */
void LED_Handle_Callback(void *config);

#endif

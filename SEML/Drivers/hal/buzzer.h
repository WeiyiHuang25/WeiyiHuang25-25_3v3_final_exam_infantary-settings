/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : buzzer.h
 * @author : SY7_yellow
 * @brief  : 蜂鸣器组件
 * @date   : 2023-10-16
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-10-16     <td>1.0         <td>SY7_yellow  <td>实现了基础功能
 * </table>
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 调用Button_Init传入蜂鸣器句柄,pwm配置
 * 2. 将Buzzer_Callback放入毫秒级定时器中断中
 * 3. 调用蜂鸣器任务让他响起来
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
#ifndef _BUZZER_H_
#define _BUZZER_H_
#include "SEML_common.h"
#include "queue.h"
#include "pwm_if.h"
/**
 * @brief 蜂鸣器任务结构体
 */
typedef struct
{
	uint32_t frec;	/**< 频率(Hz) */
	uint16_t time;	/**< 持续时间(ms) */
	uint8_t volume; /**< 音量(0-100) */
} Buzzer_Task_t;
/**
 * @brief 蜂鸣器最大任务数量
 *
 */
#define BUZZER_TASK_NUM 40
/**
 * @brief 蜂鸣器句柄结构体
 */
typedef struct
{
	s_queue task_queue;															/**< 任务队列 */
	Buzzer_Task_t task_buffer[BUZZER_TASK_NUM + 1]; /**< 任务队列缓存 */
	uint32_t timeout;																/**< 超时时间 */
	PWM_Handle_t pwm_handle;												/**< pwm句柄 */
	uint8_t task_num;																/**< 已用任务数量 */
	void *next;																			/**< 下一个蜂鸣器句柄 */
} Buzzer_t;

/**
 * @brief 初始化蜂鸣器
 * @param buzzer 蜂鸣器句柄
 * @param pwm_handle pwm句柄
 */
void Buzzer_Init(Buzzer_t *buzzer, PWM_IF_t *htim,uint32_t channel,uint32_t timer_clock_frec);

/**
 * @brief 蜂鸣器任务
 * @param config 蜂鸣器句柄
 */
void Buzzer_Handle(Buzzer_t *buzzer);

/**
 * @brief 蜂鸣器回调函数
 * @param config 定时器句柄
 */
void Buzzer_Callback(void *config);

/**
 * @brief 蜂鸣器添加任务
 * @param buzzer 蜂鸣器句柄
 * @param frec 频率(Hz)
 * @param volume 音量(0-100)
 * @param time 时间(ms)
 * @return 添加成功返回SEML_OK
 */
SEML_StatusTypeDef Buzzer_Add_Task(Buzzer_t *buzzer, uint32_t frec, uint8_t volume, uint16_t time);

/**
 * @brief 蜂鸣器任务--哔三声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef
 */
SEML_StatusTypeDef Buzzer_Task_B_B_B(Buzzer_t *buzzer, uint8_t volume);

/**
 * @brief 蜂鸣器任务--哔两声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef
 */
SEML_StatusTypeDef Buzzer_Task_B_B(Buzzer_t *buzzer, uint8_t volume);

/**
 * @brief 蜂鸣器任务--哔一声
 * @param buzzer 蜂鸣器句柄
 * @param volume 音量(0-100)
 * @return SEML_StatusTypeDef 
 */
SEML_StatusTypeDef Buzzer_Task_B(Buzzer_t *buzzer, uint8_t volume);

/**
 * @brief 获取可用蜂鸣器任务数
 */
#define Buzzer_Available_Tasks_Num(buzzer) (BUZZER_TASK_NUM - buzzer->task_num)

#endif

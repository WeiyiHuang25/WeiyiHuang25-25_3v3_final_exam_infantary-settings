/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : WS281x.h
 * @author : SY7_yellow
 * @brief  : WS281x驱动
 * @date   : 2023-9-30
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-09-30     <td>1.0         <td>SY7_yellow  <td>创建初始版本
 * </table>
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 根据驱动方式声明相对应的句柄和调用驱动初始化函数,例如tim+DMA驱动使用
 * WS281x_PWM_Driver_Init进行初始化,获得其返回值为WX281x句柄，接下来的操作都是基
 * 于该句柄.
 * 2. 使用TIM+DMA方式定时频率为800kHz,需要向TIM的HAL_TIM_PWM_PULSE_FINISHED_CB_ID
 * 注册WS281x_TIMCallback.
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
#ifndef __WS281X_H
#include "SEML_common.h"
#include "tim.h"

#define GRB (3 * 8)

typedef enum
{
	WS281x_PWM_Driver,
}WS281x_Driver_Mode_t;
typedef struct 
{
	void* Driver;
	WS281x_Driver_Mode_t Driver_mode;
}WS281x_t;

typedef struct
{
	uint16_t zero_count;
	uint16_t one_count;
	uint16_t *pixel_buffer;
	uint16_t pixel_num;
	void *htim;
	uint32_t tim_channel;
	WS281x_t WS281x;
	void *next;
} WS281x_PWM_Driver_t;

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
WS281x_t* WS281x_PWM_Driver_Init(WS281x_PWM_Driver_t *handle, uint16_t *pixel_buffer, uint16_t pixel_num, void *htim, uint32_t tim_channel, uint32_t period);

/**
 * @brief WS281x显示函数
 * @param handle WS281x句柄
 */
void WS281x_Show(WS281x_t *handle);

/**
 * @brief 将rgb通道颜色合并成ws281x报文格式
 * @param r 红
 * @param g 绿
 * @param b 蓝
 * @return uint32_t 
 */
uint32_t WS281x_Color(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief WS281x设置第n个点的颜色
 * @param handle WS281x句柄
 * @param n 位置
 * @param color 颜色
 */
void WS281x_SetColor(WS281x_t *handle, uint16_t n, uint32_t color);

/**
 * @brief 将ws281x熄灭，清空缓冲区
 * @param handle WS281x句柄
 */
void WS281x_Close(WS281x_t *handle);

/**
 * @brief WS281x定时器回调函数
 * @param htim 定时器句柄
 */
void WS281x_TIMCallback(TIM_HandleTypeDef *htim);

#endif /* __WS281X_H */

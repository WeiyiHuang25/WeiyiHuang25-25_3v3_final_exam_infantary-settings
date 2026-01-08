/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : timer.h
 * @author : SY7_yellow
 * @brief  : 软件定时器模块
 * @date   : 2023-9-6
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-09-06     <td>1.0         <td>SY7_yellow  <td>实现软件定时器
 * <tr><td>2023-09-17     <td>1.1         <td>SY7_yellow  <td>添加了软件看门狗
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * ** 软件定时器 **
 * 软件定时器是基于硬件定时器的基础上进行拓展，模拟出更多时基的定时器的一个模块,
 * 软件定时器运行模式有单次模式(定时时间到后销毁)和循环模式.
 * 定义一个Soft_Timer_t变量,使用SEML_Timer_Create配置好定时时间,运行模式,回调函数
 * 后将SEML_Timer_Callback注册(或放入)硬件定时器中断中.定时器初始化后默认是使能的,
 * 如果不需要使能可以执行SEML_Timer_Stop关闭定时器.
 * ** 软件看门狗 **
 * 软件看门狗是基于软件定时器的一个衍生,所以需要先配置好软件定时器.
 * 在要使用看门狗的句柄中加入下列定义:
 * Soft_WitchDog_t witchdog;
 * 即可使用软件看门狗,在句柄初始化函数中调用SEML_WitchDog_Register
 * 记得喂狗即可.
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
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
#ifndef _TIMER_H_
#define _TIMER_H_
#include "SEML_common.h"
/**
 * @addtogroup 系统库
 * @{
 */
/**
 * @addtogroup 软件定时器模块
 * @{
 */

/**
 * @brief 软件定时器运行模式
 */
typedef enum
{
	Timer_Single, /**< 单次模式 */
	Timer_Cycle		/**< 循环模式 */
} Timer_AutoReload;
/**
 * @brief 是否为软件定时器运行模式
 */
#define IS_TIMER_AUTORELOAD(a) ((a) == Timer_Single || (a) == Timer_Cycle)

/**
 * @brief 软件定时器运行状态
 */
typedef enum
{
	Timer_Stop, /**< 停止运行 */
	Timer_Run		/**< 开始运行 */
} Timer_Status;
/**
 * @brief 是否为软件定时器运行状态
 */
#define IS_TIMER_STATUS(a) ((a) == Timer_Stop || (a) == Timer_Run)

/**
 * @brief 软件定时器句柄
 */
typedef struct
{
	void (*callback_fun)(void *config); /**< 回调函数 */
	void *callback_config;							/**< 回调函数配置句柄 */
	uint32_t timeout_count;							/**< 定时器计数器 */
	uint32_t timeout_buckup;						/**< 定时器重装载值 */
	Timer_AutoReload mode : 1;					/**< 定时器运行模式 */
	Timer_Status status : 1;						/**< 定时器运行状态 */
	void *next;													/**< 下一个定时器 */
} Soft_Timer_t;

/**
 * @brief 软件定时器初始化
 * @param[out] hstim 软件定时器句柄
 * @param[in] mode 定时器模式
 * @param[in] timeout 定时时间(以调用的硬件定时器为时基的整数倍)
 * @param[in] callback_fun 回调函数
 * @param[out] callback_config 回调函数配置
 */
void SEML_Timer_Create(Soft_Timer_t *hstim, Timer_AutoReload mode, uint32_t timeout, void (*callback_fun)(void *config), void *callback_config);

/**
 * @brief 删除软件定时器
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Delete(Soft_Timer_t *hstim);

/**
 * @brief 软件定时器复位
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Reset(Soft_Timer_t *hstim);

/**
 * @brief 软件定时器启动
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Start(Soft_Timer_t *hstim);

/**
 * @brief 软件定时器关闭
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Stop(Soft_Timer_t *hstim);

/**
 * @brief 软件定时器回调函数
 * @param[in] config 回调函数配置
 */
void SEML_Timer_Callback(void *config);

///@} 软件定时器结束

/**
 * @addtogroup 软件看门狗
*/
/**
 * @brief 软件看门狗句柄
*/
typedef Soft_Timer_t Soft_WitchDog_t;

/**
 * @brief 软件看门狗注册
 * @note 需要在句柄中加入看门狗句柄定义(Soft_WitchDog_t witchdog)
 * @param[out] handle 要注册看门狗的句柄
 * @param[in] timeout 超时时间
 * @param[in] callback_fun 超时回调函数
 */
#define SEML_WitchDog_Register(handle,timeout,callback_fun) SEML_Timer_Create(&(handle)->witchdog,Timer_Cycle,timeout,(void (*)(void *))callback_fun,handle)

/**
 * @brief 喂狗
 * @param[out] handle 句柄
 */
#define SEML_Feed_WitchDog(handle) SEML_Timer_Reset(&(handle)->witchdog)

///@} 软件看门狗结束
///@} 系统库结束
#endif

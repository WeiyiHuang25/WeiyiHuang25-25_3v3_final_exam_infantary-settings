/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : DCDC.h
 * @author : SY7_yellow
 * @brief  : 直流变换器控制模块
 * @date   : 2023-7-15
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-07-15     <td>0.1-alpha   <td>SY7_yellow  <td>从超电代码移植过来
 * <tr><td>2023-07-25     <td>0.1         <td>SY7_yellow  <td>调试通过,代码能跑
 * <tr><td>2023-07-26     <td>0.2         <td>SY7_yellow  <td>移植了mppt算法
 * <tr><td>2024-02-08     <td>0.3         <td>SY7_yellow  <td>重构了buck-boost控制器
 * </table>
 * @todo    : 使用q16格式，尽量摆脱使用浮点型
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 在初始化的时候调用BuckBoost_Init，并且配置好参数
 * 2. 使用BuckBoost_Config设定输出电压和输出电流
 * 3. 在控制开关管的PWM定时器中断中调用BuckBoost_Loop_Control,若运算频率不够可以
 *    适当降低控制频率.注意控制频率不得高于adc的采样频率.
 * 4. 在较低频率的定时器(如100Hz定时器)中调用BuckBoost_Status_Control，通过
 *    Power_switch来控制环路的开启和关闭。
 * 5. 本库适用于buck变换器,boost变换器,四开关buck-boost变换器。
 * 6. 有些定时器(如hrtim)可能会出现占空比为0时候变成上管导通，此时需要对其进行限
 *    幅，使得cmp值>=1.
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
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 ******************************************************************************
 */
#ifndef _DCDC_H_
#define _DCDC_H_
#include "SEML_common.h"
#include "math_common.h"
#include "PID.h"
#include "math_filter.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 电源控制库
 * @{
 */
/**
 * @addtogroup 直流变换器控制
 * @{
 */
/**
 * @addtogroup 四开关buck-boost变换器控制器
 * @{
 */

/**
 * @brief 定时器比较寄存器类型
 */
typedef uint32_t TIM_Compare_Register_t;

/**
 * @brief buck-boost设定短路电流回调函数
 */
typedef void (*Set_ShortCircuit_Value_Fun_t)(float value);
/**
 * @brief 四开关buck-boost工作模式
 */
typedef enum
{
	Wait_Mode = 0,
	Buck_Mode,
	Boost_Mode,
	Buck_Boost_Mode
} BuckBoost_mode_t;

/**
 * @brief 四开关buck-boost工作状态
 */
typedef enum
{
	CV_Status = 0, /**< 恒压模式*/
	CC_Status,		 /**< 恒流模式*/
	SCP_Status		 /**< 短路保护模式 */
} BuckBoost_Status_t;

typedef enum
{
	SCP_Mode = 0, /**< 短路保护模式 */
	SCC_Mode			/**< 短路恒流模式 */
} Shortcircuit_Mode_t;
typedef struct
{
	float max_duty;											/**< 上管最大占空比(一般是定时器period寄存器的值) */
	float min_duty;											/**< 上管最小占空比(上管持续关断,下管持续导通的状态) */
	float max_transform_ratio;					/**< 最大电压变换比(一般<4,大于4效率低) */
	float V_ramp_ratio;									/**< 软起动电压增长率(伏/秒,V/s,启动的时候电压按照该增长率匀速上升) */
	float control_frec;									/**< 控制频率(主环路执行频率) */
	TIM_Compare_Register_t *boost_duty; /**< boost侧定时器比较寄存器指针(上管占空比) */
	TIM_Compare_Register_t *buck_duty;	/**< buck侧定时器比较寄存器指针(上管占空比) */
	Incremental_PID_t Vloop_PID;				/**< 电压环PID(不使用电压环请随便往积分系数填入>0的值) */
	Incremental_PID_t Iloop_PID;				/**< 电流环PID(不使用电流环请随便往积分系数填入>0的值) */
	float *Vout;												/**< 输出电压指针(不使用电压环请将其指向0值变量,并且设定电压>0) */
	float *Iout;												/**< 输出电流指针(不使用电流环请将其指向0值变量,并且设定电流>0) */
	float *Vin;													/**< 输入电流指针(不使用请指向NULL,配置后会优化缓启动和电流环电压环切换效果) */
	struct
	{
		FunctionalState reverse_current : 1;			 /**< 反向电流状态 */
		Shortcircuit_Mode_t shortcircuit_mode : 1; /**< 短路工作模式 */
	} flag;
} BuckBoost_Config_t;
/**
 * @brief 四开关控制器dcdc参数结构体
 */
typedef struct
{
	BuckBoost_Config_t config;
	float expect_Vout;
	float Set_Iout;
	float Set_Vout;
	float Vloop_max;
	uint8_t count;
	BuckBoost_mode_t mode : 2;		 /**< 控制器工作模式 */
	BuckBoost_Status_t status : 2; /**< 控制器工作状态 */
	Set_ShortCircuit_Value_Fun_t Set_ShortCircuit;
} BuckBoost_t;

/**
 * @brief 四开关buck-boost控制器初始化
 * @param[in,out] dcdc 电源环路句柄
 * @param[in] config 电源环路配置
 */
void BuckBoost_Init(BuckBoost_t *dcdc, BuckBoost_Config_t *config);

/**
 * @brief 开启四开关buck-boost环路
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Start(BuckBoost_t *dcdc);
/**
 * @brief 关闭四开关buck-boost环路
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Stop(BuckBoost_t *dcdc);

/**
 * @brief 设置四开关buck-boost的输出电压和输出电流
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Setting(BuckBoost_t *dcdc, float set_Vout, float set_Iout);

/**
 * @brief 四开关buck-boost电源环路控制
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Control_Loop(BuckBoost_t *dcdc);

/**
 * @brief 四开关buck-boost控制器短路处理函数
 * @param dcdc 电源环路句柄
 */
void BuckBoost_ShortCircuit_Handle(BuckBoost_t *dcdc);

/**
 * @brief 注册buck-boost设定短路电流回调函数
 * 该函数是设定模拟看门狗阈值的函数
 * @note 在模拟看门狗越界中断中调用BuckBoost_ShortCircuit_Handle进行短路处理
 * @param[in,out] dcdc 电源环路句柄
 * @param[in] Set_ShortCircuit 注册短路电流回调函数
 */
void BuckBoost_Register_Set_ShortCircuit_Callback(BuckBoost_t *dcdc, Set_ShortCircuit_Value_Fun_t Set_ShortCircuit);

/// @} 四开关buck-boost算法结束
/**
 * @addtogroup MPPT算法
 * @{
 */

typedef struct
{
	float Pin;				 /**< 输入功率 */
	float lest_Pin;		 /**< 上一次输入功率 */
	float step_longth; /**< 步长 */
	float max_duty;		 /**< 最大占空比 */
	float min_duty;		 /**< 最小占空比 */
	float mppt_duty;	 /**< mppt占空比 */
	enum
	{
		mppt_add = 0,
		mppt_sub
	} mppt_direction; /**< 占空比移动方向 */
} mppt_config_t;

/**
 * @brief 最大功率点追踪初始化
 * @param[out] mppt_config 最大功率点配置结构体
 * @param[in] max_duty 最大占空比
 * @param[in] min_duty 最小占空比
 * @param[in] step_longth 每次迭代步长
 */
void mppt_Init(mppt_config_t *mppt_config, float max_duty, float min_duty, float step_longth);

/**
 * @brief 最大功率点追踪
 * @param[in,out] mppt_config 最大功率点配置结构体
 * @param[in] Vin 输入电压
 * @param[in] Iin 输入电流
 * @return mppt占空比(上管占空比)
 */
float mppt(mppt_config_t *mppt_config, float Vin, float Iin);

/// @} MPPT算法结束
/// @} 直流变换器控制结束
/// @} 电源控制库结束
/// @} 解算库结束
#endif

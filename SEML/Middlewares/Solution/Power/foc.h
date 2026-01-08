/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : foc.h
 * @author : SY7_yellow
 * @brief  : 磁场定向控制模块
 * @date   : 2023-7-15
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-07-15     <td>0.1-alpha   <td>SY7_yellow  <td>实现了park,clacke,svpwm
 * <tr><td>2023-07-21     <td>0.2         <td>SY7_yellow  <td>添加了spwm
 * <tr><td>2023-11-14     <td>0.3         <td>SY7_yellow  <td>添加了foc控制环路和传感器标定
 * </table>
 * @todo    : 使用q16格式，尽量摆脱使用浮点型
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 使用坐标变换调用对应函数即可，svpwm算法的只需调用svpwm()。
 * 2. 使用电机foc控制环路需要将Motor_FOC_Loop()放入高频率定时器中断,进行初始化配置.
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
#ifndef _FOC_H_
#define _FOC_H_
#include "SEML_common.h"
#include "math_common.h"
#include "Motor.h"
#include "PID.h"
#define SQRT_3 1.7320508076f
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 电源控制库
 * @{
 */
/**
 * @addtogroup FOC算法
 * @{
 */

/**
 * @addtogroup 坐标变换
 * @{
 */
/// @brief dq坐标系结构体
typedef struct
{
	float d;
	float q;
} dq_frame_t;
/// @brief αβ坐标系结构体
typedef struct
{
	float alpha;
	float beta;
} alpha_beta_frame_t;
/// @brief ABC坐标系结构体
typedef struct
{
	float A;
	float B;
	float C;
} ABC_frame_t;
/**
 * @brief clarke变换 将ABC坐标系转换成αβ坐标系
 * @param ABC_frame ABC坐标系的坐标值
 * @param alpha_beta_frame αβ坐标系的坐标值指针
 * @attention 该变换为等幅值变换，等向量需乘以3/2，等功率需要乘sqrt(27/8)
 */
_FAST void clarke_transform(const ABC_frame_t ABC_frame, alpha_beta_frame_t *alpha_beta_frame);

/**
 * @brief clarke逆变换 将αβ坐标系转换成ABC坐标系
 * @param[in] alpha_beta_frame αβ坐标系的坐标值
 * @param[out] ABC_frame ABC坐标系的坐标值指针
 * @attention 该变换为等幅值变换，等向量需乘以2/3，等功率需要乘sqrt(8/27)
 */
_FAST void inv_clarke_transform(alpha_beta_frame_t alpha_beta_frame, ABC_frame_t *ABC_frame);

/**
 * @brief park变换 将αβ坐标系转换成dq坐标系
 * @param[in] alpha_beta_frame αβ坐标系的坐标值
 * @param[in] wt 当前转子磁通位置
 * @param[out] dq_frame dq坐标系的坐标值指针
 */
_FAST void park_transform(alpha_beta_frame_t alpha_beta_frame, float wt, dq_frame_t *dq_frame);

/**
 * @brief park逆变换 将αβ坐标系转换成dq坐标系
 * @param[in] dq_frame dq坐标系的坐标值
 * @param[in] wt 当前转子磁通位置
 * @param[out] alpha_beta_frame αβ坐标系的坐标值指针
 */
_FAST void inv_park_transform(dq_frame_t dq_frame, float wt, alpha_beta_frame_t *alpha_beta_frame);

/// @} 坐标变换结束
/**
 * @addtogroup svpwm算法
 * @{
 */

/**
 * @brief 矢量扇区判断
 * @param[in] U_alpha_beta Uαβ
 * @return N值，与所在扇区一一对应
 * @retval 与所在扇区映射关系为(前N后扇区)(3,1)(1,2)(5,3)(4,4)(6,5)(2,6)
 */
_FAST uint8_t vector_sector_judgment(alpha_beta_frame_t U_alpha_beta);

/**
 * @brief 计算矢量作用时间
 * @param[in] U_alpha_beta Uαβ
 * @param[in] N N值
 * @param[in] Udc 直流电压
 * @param[out] T 非零矢量作用时间数组
 */
_FAST void calculate_vector_action_time(alpha_beta_frame_t U_alpha_beta, uint8_t N, float Udc, float T[2]);
/**
 * @brief 扇区矢量作用时间切换
 * @param[in] T 非零矢量作用时间数组
 * @param[in] N N值
 * @param[out] Tcm 调制波数组，范围[0,1]
 */
_FAST void sector_vector_switching(float T[2], uint8_t N, ABC_frame_t *Tcm);

/**
 * @brief 空间矢量调制算法
 * @param[in] U_alpha_beta Uα,Uβ电压值
 * @param[in] Udc 母线电压
 * @param[out] Tcm 三相占空比指针
 */
_FAST void svpwm(alpha_beta_frame_t U_alpha_beta, float Udc, ABC_frame_t *Tcm);

/**
 * @brief 单相正弦调制算法
 * @param[in] U_alpha_beta Uα,Uβ电压值
 * @param[in] Udc 母线电压
 * @param[out] Tcm 三相占空比指针
 */

/**
 * @brief 单相正弦调制算法
 * @param[in] n 幅值
 * @param[in] wt 当前转子磁通位置
 * @param[out] Tcm 三相占空比指针
 */
void Single_Phase_spwm(float n, float wt, ABC_frame_t *Tcm);

/**
 * @brief 三相正弦调制算法
 * @param[in] U_alpha_beta Uα,Uβ电压值
 * @param[in] Udc 母线电压
 * @param[out] Tcm 三相占空比指针
 */
void Three_Phase_spwm(alpha_beta_frame_t U_alpha_beta, float Udc, ABC_frame_t *Tcm);

/// @} svpwm算法结束
/**
 * @addtogroup 电机foc控制
 * @{
 */

typedef struct
{
	float electrical_angle;		 /**< 电角度 */
	float zero_electric_angle; /**< 绝对零电角 */
	float zero_i_angle;				 /**< 电流零电角 */
	float *angle;							 /**< 实际角度 */
	struct
	{
		uint32_t *A;
		uint32_t *B;
		uint32_t *C;
	} timer_phase;			/**< 定时器输出比较寄存器指针 */
	uint16_t timer_ref; /**< 定时器参考值 */
} Motor_Driver_t;

typedef struct
{
	int pole_pairs;						/**< 极对数 */
	float d_phase_inductance; /**< d轴相电感 */
	float q_phase_inductance; /**< q轴相电感 */
	float phase_resistance;		/**< 相电阻 */
} Motor_Parameter_t;

typedef struct
{
	Motor_t motor;					 /**< 电机句柄 */
	Motor_Driver_t driver;	 /**< 电机驱动 */
	Motor_Parameter_t param;
	PIDConfig_t PID_id;			 /**< d轴电流pid */
	PIDConfig_t PID_iq;			 /**< q轴电流pid */
	dq_frame_t i_dq;				 /**< dq轴电流 */
	dq_frame_t u_dq;				 /**< dq轴电压 */
	ABC_frame_t Tcm;				 /**< 输出占空比 */
	float *Ia;							 /**< A相电流 */
	float *Ib;							 /**< B相电流 */
	float *electrical_angle; /**< 电角度 */
	float expect_iq;				 /**< 期望转矩电流 */
	struct
	{
		uint32_t OpenLoop : 1;
	} status;
} Motor_FOC_Control_t;
typedef enum
{
	FOC_Current_Sample_AB,
	FOC_Current_Sample_AC,
	FOC_Current_Sample_BC
} Motor_FOC_Current_Sample_t;
/**
 * @brief FOC闭环控制环路
 * @param foc foc句柄
 * @param I_abc abc相电流
 */
void Motor_FOC_Loop(Motor_FOC_Control_t *foc);

/**
 * @brief 标定foc位置传感器和极对数
 * @param foc foc句柄
 * @param current_sample 电流传感器安装位置
 */
SEML_StatusTypeDef Motor_Calibration_Sensor(Motor_FOC_Control_t *foc, Motor_FOC_Current_Sample_t current_sample);
/// @} 电机foc控制
/// @} FOC算法结束
/// @} 电源控制库结束
/// @} 解算库结束
#endif
/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : chassis.h
 * @author : Chad SY7_yellow
 * @brief  : 底盘运动学解算
 * @date   : 2023-9-12
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-03-07     <td>0.1         <td>Chad  			<td>实现麦克纳姆轮和全向
 * 																													  轮的逆向运动学
 * <tr><td>2023-09-14     <td>1.0-alpha   <td>SY7_yellow  <td>将麦克纳姆逆向运动学
 * 																														解算移植了过来,添加了
 * 																														麦克纳姆正向运动学解算
 * <tr><td>2023-09-14     <td>1.1-alpha   <td>SY7_yellow  <td>将四轮全向轮逆向运动学
 * 																														解算移植了过来,添加了
 * 																														四轮全向轮正向运动学解算
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 *
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
#ifndef _CHASSIS_H_
#define _CHASSIS_H_
#include "SEML_common.h"
#include "math_common.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 底盘运动学解算
 * @{
*/
/**
 * @brief 底盘轮子编号
 */
typedef enum
{
	CHASSIS_FL = 0, /**< 左前轮 */
	CHASSIS_FR = 1, /**< 右前轮 */
	CHASSIS_BL = 2, /**< 左后轮 */
	CHASSIS_BR = 3, /**< 右后轮 */
} Chassis_Wheel_Number_t;

/**
 * @brief 底盘运动状态
 */
typedef struct
{
	float vx; /**< 底盘前后移动速度,单位:m/s */
	float vy; /**< 底盘左右移动速度,单位:m/s */
	float vw; /**< 底盘旋转速度,单位:rad/s */
} Chassis_Velocity_t;
/**
 * @brief 底盘机械参数配置
 */
typedef struct
{
	float wheel_track;								 /**< 轮距 同一轴上两个轮子中心线距离,单位:mm */
	float wheel_base;									 /**< 轴距 两个电机轴,单位:mm */
	float wheel_radius;								 /**< 轮子半径,单位:mm */
	float wheel_motor_reduction_ratio; /**< 轮子电机减速比 */
	int8_t rotation_direction;				 /**<  旋转方向 +1或-1*/
} Chassis_Mechanical_Param_t;

typedef void (*Inverse_Kinematics_Fun_t)(void *chassis, Chassis_Velocity_t *car_velocity, float wheel_rpm[4]);
typedef void (*Forward_Kinematics_Fun_t)(void *chassis, float wheel_rpm[4], Chassis_Velocity_t *car_velocity);

typedef struct
{
	Chassis_Mechanical_Param_t mechanical_param;		 /**< 底盘机械参数 */
	float wheel_rpm_ratio;													 /**< 轮子转动系数 */
	float export_ratio;													 		 /**< 轮子输出系数 */
	Inverse_Kinematics_Fun_t inverse_kinematics_fun; /**< 逆向运动学回调函数 */
	Forward_Kinematics_Fun_t forward_kinematics_fun; /**< 正向运动学回调函数 */
} Chassis_Kinematic_t;


/**
 * @brief 底盘逆向运动学解算
 * @note 根据运动状态求解出每个轮子的子状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] car_velocity 车辆运动状态
 * @param[out] wheel_rpm 轮子转速(deg/s)
*/
void Chassis_Inverse_Kinematics(Chassis_Kinematic_t *chassis, Chassis_Velocity_t *car_velocity, float wheel_rpm[4]);

/**
 * @brief 底盘正向运动学解算
 * @note 根据每个轮子的子状态求解出运动状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] wheel_rpm 轮子转速(deg/s)
 * @param[out] car_velocity 车辆运动状态
*/
void Chassis_Forward_Kinematics(Chassis_Kinematic_t *chassis, float wheel_rpm[4], Chassis_Velocity_t *car_velocity);
/**
 * @addtogroup 麦克纳姆轮运动学解算
 * @{
*/
/**
 * @brief 麦克纳姆轮运动学模型初始化
 * @param chassis 底盘运动学句柄
 * @param mechanical_param 底盘机械参数
 */
void Mecanum_Wheel_Kinematics_Init(Chassis_Kinematic_t *chassis, Chassis_Mechanical_Param_t mechanical_param);

/**
 * @brief 麦克纳姆轮逆向运动学解算
 * @note 根据运动状态求解出每个轮子的子状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] car_velocity 车辆运动状态
 * @param[out] wheel_rpm 轮子转速(deg/s)
 */
void Mecanum_Wheel_Inverse_Kinematics(Chassis_Kinematic_t *chassis, Chassis_Velocity_t *car_velocity, float wheel_rpm[4]);

/**
 * @brief 麦克纳姆轮正向运动学解算
 * @note 根据每个轮子的子状态求解出运动状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] wheel_rpm 轮子转速(deg/s)
 * @param[out] car_velocity 车辆运动状态
 */
void Mecanum_Wheel_Forward_Kinematics(Chassis_Kinematic_t *chassis, float wheel_rpm[4], Chassis_Velocity_t *car_velocity);
/// @} 麦克纳姆轮运动学解算
/**
 * @addtogroup 四轮全向轮轮运动学解算
 * @{
*/

/**
 * @brief 全向轮运动学模型初始化
 * @param chassis 底盘运动学句柄
 * @param mechanical_param 底盘机械参数
 */
void Four_Omni_Wheel_Kinematics_Init(Chassis_Kinematic_t *chassis, Chassis_Mechanical_Param_t mechanical_param);

/**
 * @brief 四轮全向轮逆向运动学解算
 * @note 根据运动状态求解出每个轮子的子状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] car_velocity 车辆运动状态
 * @param[out] wheel_rpm 轮子转速(deg/s)
 */
void Four_Omni_Wheel_Inverse_Kinematics(Chassis_Kinematic_t *chassis, Chassis_Velocity_t *car_velocity, float wheel_rpm[4]);

/**
 * @brief 四轮麦克纳姆轮正向运动学解算
 * @note 根据每个轮子的子状态求解出运动状态
 * @param[in] chassis 底盘运动学句柄
 * @param[in] wheel_rpm 轮子转速(deg/s)
 * @param[out] car_velocity 车辆运动状态
 */
void Four_Omni_Wheel_Forward_Kinematics(Chassis_Kinematic_t *chassis, float wheel_rpm[4], Chassis_Velocity_t *car_velocity);

/// @} 四轮全向轮运动学解算
/// @} 底盘运动学解算
/// @} 解算库
#endif

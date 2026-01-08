#ifndef _ROBO_GIMBAL_H_
#define _ROBO_GIMBAL_H_
#include "SEML_common.h"
#include "Robo_common.h"
#include "Dji_Motor.h"
#include "PID.h"
#include "EzTuner.h"

#define YAW_DIRECTION 1		// 电机和(执行机构在模型中定义的旋转方向)的关系（1或-1），同向为1
#define PITCH_DIRECTION 1
#define GIMBAL_YAW_OFFSET -1.9f	// 云台初始化正对齐的时候使用的yaw轴正中心量
#define GIMBAL_PITCH_OFFSET 0.0f	// 云台初始化正对齐的时候使用的pitch轴正中心量

typedef struct 
{
	Motor_t *motor;
	PIDConfig_t speed_PID;
	PIDConfig_t position_PID;
	PIDConfig_t AA_PID;

	float send_data;
	float expect_speed;
	float expect_angle;
	float expect_AA;
}Gimbal_Joint_t;

typedef struct 
{
	Gimbal_Joint_t yaw;
	Gimbal_Joint_t pitch;
	const Euler_Data_t *imu;
}Task_Gimbal_t;

/**
 * @brief 云台任务初始化
 * 该函数执行云台任务相关的初始化操作
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
void Gimbal_Init(void);

/**
 * @brief 云台任务
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
void Gimbal_Task(void *conifg);

#endif

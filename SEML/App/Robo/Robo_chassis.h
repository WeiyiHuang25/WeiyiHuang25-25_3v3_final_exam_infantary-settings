#ifndef _ROBO_CHASSIS_H_
#define _ROBO_CHASSIS_H_
#include "SEML_common.h"
#include "Robo_Common.h"
#include "chassis.h"
#include "Dji_Motor.h"
#include "PID.h"
#include "DR16_Remote.h"
#include "main.h"
#include "reference.h"
#include "can_if.h"
#include "usart.h"
#include "EzTuner.h"

typedef struct
{
	Motor_t *motor;
	PIDConfig_t speed_PID;
	float send_data;
}driver_t;

typedef struct
{
	Chassis_Kinematic_t solve;						/**< 底盘运动学解算句柄 */
	Chassis_Velocity_t expect_speed;			/**< 期望速度 */
	Chassis_Velocity_t observation_speed; /**< 观测数据 */
	Chassis_Mode_t mode;

	driver_t driver[4];

	float expect_driver_speed[4];

	PIDConfig_t yaw_follow_PID;
	PIDConfig_t buffer_PID;

	Reference_t referee;
} Task_Chassis_t;


/**
 * @brief 底盘任务初始化
 * 该函数执行底盘任务相关的初始化操作
 */
void Chassis_Init(void);
/**
 * @brief 底盘任务
 */
void Chassis_Task(void *conifg);

extern Task_Chassis_t chassis;

#endif
#ifndef _ROBO_SHOOT_H_
#define _ROBO_SHOOT_H_
#include "SEML_common.h"
#include "Robo_common.h"
#include "Dji_Motor.h"
#include "PID.h"
#include "EzTuner.h"

#define dial_add 180

typedef struct 
{
	Motor_t *motor;
    PIDConfig_t speed_PID;
    float send_data;
	float expect_speed;
}Shoot_friction_t;

typedef struct 
{
	Motor_t *motor;
	PIDConfig_t speed_PID;
	PIDConfig_t position_PID;
	float send_data;
	float expect_speed;
	float expect_angle;
}Shoot_Dial_t;

typedef struct
{
    Shoot_friction_t fric_L;
    Shoot_friction_t fric_R;
    Shoot_friction_t fric_M;
    Shoot_Dial_t dial;
		Shoot_Status_t status;
}Task_Shoot_t;

/**
 * @brief 发射机构任务初始化
 * 该函数执行发射机构任务相关的初始化操作
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
void Shoot_Init(void);

/**
 * @brief 发射机构任务
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
void Shoot_Task(void *conifg);
#endif

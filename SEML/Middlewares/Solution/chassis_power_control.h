#ifndef CHASSIS_POWER_CONTROL_H
#define CHASSIS_POWER_CONTROL_H

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
#include "Robo_Chassis.h"

typedef struct
{
	int current_power;
	int power_buffer;
	int max_power;
	float motor_speed[4];
	// PIDConfig_t motor_speed_pid[4];
	float motor_send_data[4];
	PIDConfig_t buffer_pid;
} power_control_t;

extern void power_control(Task_Chassis_t* chassis, float motor_send_data[4], float realtime_power);

#endif
#include "Motor.h"
/**
 * @brief 电机句柄初始化
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param Motor_Send_Data 电机发送回调函数
 * @param motor_config 电机配置
 */
void Motor_Init(Motor_t *motor, Motor_Send_Data_t Motor_Send_Data, void *motor_config)
{
	motor->position = 0;
	motor->speed = 0;
	motor->temperature = 0;
	motor->torque = 0;
	motor->Send_Data = Motor_Send_Data;
	motor->config = motor_config;

	motor->Position_Loop = NULL;
	motor->position_loop_config = NULL;
	motor->Speed_Loop = NULL;
	motor->speed_loop_config = NULL;
}

/**
 * @brief 电机位置环初始化
 * @param motor 电机句柄
 * @param Motor_Position_Loop 电机位置环设置回调函数
 * @param motor_position_loop_config 电机位置环句柄
 */
void Motor_Position_Loop_Config(Motor_t *motor, Motor_Send_Data_t Motor_Position_Loop, void *motor_position_loop_config)
{
	assert_param(Motor_Position_Loop != NULL);
	motor->Position_Loop = Motor_Position_Loop;
	motor->position_loop_config = motor_position_loop_config;
}

/**
 * @brief 电机速度环初始化
 * @param motor 电机句柄
 * @param Motor_Position_Loop 电机速度环设置回调函数
 * @param motor_position_loop_config 电机速度环句柄
 */
void Motor_Speed_Loop_Config(Motor_t *motor, Motor_Send_Data_t Motor_Speed_Loop, void *motor_speed_loop_config)
{
	assert_param(Motor_Speed_Loop != NULL);
	motor->Speed_Loop = Motor_Speed_Loop;
	motor->speed_loop_config = motor_speed_loop_config;
}

/**
 * @brief 电机速度环控制器
 * @param motor 电机句柄
 * @param data 期望速度
 */
void Motor_Speed_Loop_Controller(Motor_t *motor, float data)
{
	assert_param(motor != NULL);
	assert_param(motor->speed_loop_config != NULL);
	SISO_Controller_t *speed_loop = motor->speed_loop_config;
	// 速度环
	float expect_torque = speed_loop->Callback_Fun(speed_loop->config, data, motor->speed);
	// 发送转矩
	Motor_Send_Data(motor, expect_torque);
}

/**
 * @brief 电机位置环控制器
 * @param motor 电机句柄
 * @param data 期望位置
 */
void Motor_Position_Loop_Controller(Motor_t *motor, float data)
{
	assert_param(motor != NULL);
	assert_param(motor->position_loop_config != NULL);
	assert_param(motor->speed_loop_config != NULL);
	SISO_Controller_t *position_loop = motor->position_loop_config;
	SISO_Controller_t *speed_loop = motor->speed_loop_config;
	static float expect_position, expect_speed, expect_torque;
	expect_position = Zero_Crossing_Process(360.0f, data, motor->position);
	// 位置环
	expect_speed = position_loop->Callback_Fun(position_loop->config, expect_position, motor->position);
	// 速度环
	expect_torque = speed_loop->Callback_Fun(speed_loop->config, expect_speed, motor->speed);
	// 发送转矩
	Motor_Send_Data(motor, expect_torque);
}

/**
 * @brief 电机速度环控制
 * @param motor 电机句柄
 * @param data 发送数据
 */
void Motor_Speed_Ctrl(Motor_t *motor, float data)
{
	assert_param(motor->Speed_Loop != NULL);
	motor->Speed_Loop(motor, data);
}

/**
 * @brief 电机位置环控制
 * @param motor 电机句柄
 * @param data 发送数据
 */
void Motor_Position_Ctrl(Motor_t *motor, float data)
{
	assert_param(motor->Position_Loop != NULL);
	motor->Position_Loop(motor, data);
}

/**
 * @brief 电机句柄发送函数
 * @param motor 电机句柄
 * @param data 发送数据
 */
inline void Motor_Send_Data(Motor_t *motor, float data)
{
	assert_param(motor->Send_Data != NULL);
	motor->Send_Data(motor->config, data);
}

/**
 * @brief 电机位置更新函数
 * @param motor 电机句柄
 * @param new_position 新数据
 */
void Position_Update(Motor_t *motor, float new_position)
{
	float temp;
	temp = new_position - motor->position;
	if (abs(temp) > PI) // 转过半圈
	{
		if (temp < 0)
			motor->inc_position += 2 * PI + temp;
	}
	else
		motor->inc_position += temp;
	motor->position = new_position;
}

#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "SEML_common.h"
#include "PID.h"
/**
 * @brief 电机发送回调函数
 */
typedef void (*Motor_Send_Data_t)(void *config, float data);

/**
 * @brief 电机句柄结构体
 */
typedef struct
{
	float gear_ratio;
	float speed_gear;
	float position_gear;
	float position;									/**< 位置(rad) */
	float speed;								    /**< 速度(rad/s) */
	float torque;								    /**< 扭矩(N*m) */
	float inc_position;
	uint8_t temperature;						    /**< 温度 */
	void *config;								    /**< 电机配置 */
	Motor_Send_Data_t Send_Data;		            /**< 电机设置函数 */
	Motor_Send_Data_t Speed_Loop;		            /**< 速度环设置函数 */
	void *speed_loop_config;				        /**< 速度环设置句柄 */
	Motor_Send_Data_t Position_Loop;                /**< 位置环设置函数 */
	void *position_loop_config;			            /**< 位置环设置句柄 */
} Motor_t;

/**
 * @brief 电机句柄初始化
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param Motor_Send_Data 电机发送回调函数
 * @param motor_config 电机配置
 */
void Motor_Init(Motor_t *motor, Motor_Send_Data_t Motor_Send, void *motor_config);

/**
 * @brief 电机速度环初始化
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param Motor_Position_Loop 电机速度环设置回调函数
 * @param motor_position_loop_config 电机速度环句柄
 */
void Motor_Speed_Loop_Config(Motor_t *motor, Motor_Send_Data_t Motor_Position_Loop, void *motor_position_loop_config);

/**
 * @brief 电机位置环初始化
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param Motor_Position_Loop 电机位置环设置回调函数
 * @param motor_position_loop_config 电机位置环句柄
 */
void Motor_Position_Loop_Config(Motor_t *motor, Motor_Send_Data_t Motor_Speed_Loop, void *motor_speed_loop_config);

/**
 * @brief 电机速度环控制
 * @param motor 电机句柄
 * @param data 发送数据
 */
void Motor_Speed_Ctrl(Motor_t *motor, float data);
/**
 * @brief 电机位置环控制
 * @param motor 电机句柄
 * @param data 发送数据
 */
void Motor_Position_Ctrl(Motor_t *motor, float data);
/**
 * @brief 电机句柄发送函数
 * @param motor 电机句柄
 * @param data 发送数据
 */
void Motor_Send_Data(Motor_t *motor, float data);

/**
 * @brief 电机速度环控制器
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param data 期望速度
 */
void Motor_Speed_Loop_Controller(Motor_t *motor, float data);

/**
 * @brief 电机位置环控制器
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param data 期望位置
 */
void Motor_Position_Loop_Controller(Motor_t *motor, float data);

/**
 * @brief 电机位置更新函数
 * 该函数由各个电机驱动调用,无需用户调用
 * @param motor 电机句柄
 * @param new_position 新数据
 */
void Position_Update(Motor_t *motor, float new_position);

/**
 * @brief 获取电机位置(rad)
 */
#define Get_Motor_Position_Data(motor) ((motor)->position)

/**
 * @brief 获取电机增量位置(rad)
 */
#define Get_Motor_Inc_Position_Data(motor) ((motor)->inc_position)

/**
 * @brief 获取电机速度(rad/s)
 */
#define Get_Motor_Speed_Data(motor) ((motor)->speed)

/**
 * @brief 获取电机转矩
 */
#define Get_Motor_Torque_Data(motor) ((motor)->torque)

/**
 * @brief 获取电机温度
 */
#define Get_Motor_Temperature_Data(motor) ((motor)->temperature)

/**
 * @brief rad/s转rpm
 */
#define Rads2RPM(rad_data) ((rad_data) * 60 / (2 * PI))

/**
 * @brief rpm转rad/s
 */
#define RPM2Rads(rad_data) ((rad_data) * (2 * PI) / 60)

#endif

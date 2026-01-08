#ifndef __HT_MOTOR__
#define __HT_MOTOR__

#include "can_if.h"
#include "timer.h"
#include "Motor.h"

#define P_MIN -95.5f    // Radians
#define P_MAX 95.5f        
#define V_MIN -45.0f    // Rad/s
#define V_MAX 45.0f
#define KP_MIN 0.0f     // N-m/rad
#define KP_MAX 500.0f
#define KD_MIN 0.0f     // N-m/rad/s
#define KD_MAX 5.0f
#define T_MIN -18.0f
#define T_MAX 18.0f

#define HT_MOTOR_TIMEOUT 1000
#define HT_SPEED_BIAS -0.0109901428f // 电机速度偏差,单位rad/s

typedef enum
{
	HT_Motor_MIT_Mode = 0,	    /**< MIT模式 */
	HT_Motor_PosSpeed_Mode,     /**< 位置速度模式 */
	HT_Motor_Speed_Mode			/**< 速度模式 */
} HT_Motor_Ctrl_Mode_t;

/**
 * @brief HT电机发送配置结构体
 */
typedef struct
{
	Can_Handle_t *can_handle;      /**< 消息发送句柄 */
	uint16_t id;				   /**< 发送数据包id */
	uint32_t timestamp;
} HT_Motor_tx_t;

/**
 * @brief HT电机句柄
 */
typedef struct
{
	Motor_t motor;
	uint8_t error_code;
	float max_speed;
	HT_Motor_tx_t tx_config;
	Soft_WitchDog_t witchdog;
	float machine_angle;
	float machine_speed;
	float machine_torque;
	float HT_Send_Data
} HT_Motor_t;

/**
 * @brief HT电机初始化
 * @param HT_motor HT电机句柄
 * @param ctrl_mode 控制模式
 * @param master_id 电机反馈报文主id
 * @param motor_id 电机的控制报文id
 */
Motor_t *HT_Motor_Init(HT_Motor_t *ht_motor, HT_Motor_Ctrl_Mode_t ctrl_mode, uint16_t master_id, uint16_t motor_id, Can_Handle_t *can_handle);

/**
 * @brief  位置速度模式控下控制帧
 * @param  ht_motor  HT电机句柄
 * @param  _pos   位置给定
 * @param  _vel   最大速度给定
 */
void HT_Motor_PosSpeed_Ctrl(HT_Motor_t *ht_motor, float _pos, float _vel);

/**
 * @brief  MIT模式控下控制帧
 * @param  ht_motor   HT电机句柄
 * @param  _pos   位置给定
 * @param  _vel   速度给定
 * @param  _KP    位置比例系数
 * @param  _KD    位置微分系数
 * @param  _torq  转矩给定值
 */
void HT_Motor_MIT_Ctrl(HT_Motor_t *ht_motor, float _pos, float _vel, float _KP, float _KD, float _torq);

/**
 * @brief  速度模式控下控制帧
 * @param  ht_motor  HT电机句柄
 * @param  _vel   速度给定
 */
void HT_Motor_Speed_Ctrl(HT_Motor_t *ht_motor, float _vel);

/**
 * @brief HT电机转矩控制
 * @param ht_motor HT电机句柄
 * @param data 转矩
 */
void HT_Send_Torque(HT_Motor_t *ht_motor, float data);

/**
 * @brief  电机使能
 * @param[in]  ht_motor  HT电机句柄
 */
void HT_Motor_Enable(HT_Motor_t *ht_motor);

/**
 * @brief  电机失能
 * @param[in]  ht_motor  HT电机句柄
 */
void HT_Motor_Disable(HT_Motor_t *ht_motor);

/**
 * @brief  清除故障
 * @param[in]  ht_motor  HT电机句柄
 */
void HT_Motor_Clear_Err(HT_Motor_t *ht_motor);

/**
 * @brief  电机保存零点
 * @param[in]  ht_motor  HT电机句柄
 */
void HT_Motor_Save_Position_Zero(HT_Motor_t *ht_motor);

#endif /* __HT_MOTOR__ */
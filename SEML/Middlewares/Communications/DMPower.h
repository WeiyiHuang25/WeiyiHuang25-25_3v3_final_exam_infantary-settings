#ifndef __DMPOWER__
#define __DMPOWER__
#include "can_if.h"
#include "timer.h"
#include "Motor.h"
#define P_MIN -12.5 // 位置最小值
#define P_MAX 12.5	// 位置最大值
#define V_MIN -45		// 速度最小值
#define V_MAX 45		// 速度最大值
#define KP_MIN 0		// Kp最小值
#define KP_MAX 500	// Kp最大值
#define KD_MIN 0		// Kd最小值
#define KD_MAX 5		// Kd最大值
#define T_MIN -18		// 转矩最大值
#define T_MAX 18		// 转矩最小值

#define DJI_MOTOR_TIMEOUT 1000

typedef enum
{
	DM_Motor_MIT_Mode = 0,	/**< MIT模式 */
	DM_Motor_PosSpeed_Mode, /**< 位置速度模式 */
	DM_Motor_Speed_Mode			/**< 速度模式 */
} DM_Motor_Ctrl_Mode_t;

/**
 * @brief 大疆电机发送配置结构体
 */
typedef struct
{
	Can_Handle_t *can_handle; /**< 消息发送句柄 */
	uint16_t id;							/**< 发送数据包id */
	uint32_t timestamp;
} DM_Motor_tx_t;

typedef struct
{
	Motor_t motor;
	uint8_t error_code;
	float max_speed;
	DM_Motor_tx_t tx_config;
	Soft_WitchDog_t witchdog;
} DM_Motor_t;

/**
 * @brief 达妙电机初始化
 * @param dm_motor 达妙电机句柄
 * @param ctrl_mode 控制模式
 * @param master_id 电机反馈报文主id
 * @param motor_id 电机的控制报文id
 */
Motor_t *DM_Motor_Init(DM_Motor_t *dm_motor, DM_Motor_Ctrl_Mode_t ctrl_mode, uint16_t master_id, uint16_t motor_id, Can_Handle_t *can_handle, float max_speed);

/**
 * @brief  位置速度模式控下控制帧
 * @param  dm_motor  达妙电机句柄
 * @param  _pos   位置给定
 * @param  _vel   最大速度给定
 */
void DM_Motor_PosSpeed_Ctrl(DM_Motor_t *dm_motor, float _pos, float _vel);

/**
 * @brief  MIT模式控下控制帧
 * @param  dm_motor   达妙电机句柄
 * @param  _pos   位置给定
 * @param  _vel   速度给定
 * @param  _KP    位置比例系数
 * @param  _KD    位置微分系数
 * @param  _torq  转矩给定值
 */
void DM_Motor_MIT_Ctrl(DM_Motor_t *dm_motor, float _pos, float _vel, float _KP, float _KD, float _torq);

/**
 * @brief  速度模式控下控制帧
 * @param  dm_motor  达妙电机句柄
 * @param  _vel   速度给定
 */
void DM_Motor_Speed_Ctrl(DM_Motor_t *dm_motor, float _vel);

/**
 * @brief  电机使能
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Enable(DM_Motor_t *dm_motor);

/**
 * @brief  电机失能
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Disable(DM_Motor_t *dm_motor);

/**
 * @brief  清除故障
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Clear_Err(DM_Motor_t *dm_motor);

/**
 * @brief  电机保存零点
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Save_Position_Zero(DM_Motor_t *dm_motor);

#endif /* __DMPOWER__ */

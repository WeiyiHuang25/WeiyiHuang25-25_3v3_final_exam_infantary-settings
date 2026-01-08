// #include "Motor.h"
// #include "timer.h"
// #include "Motor.h"
// #include "can_if.h"

// #define MIT_P_MIN -12.5 // 位置最小值
// #define MIT_P_MAX 12.5	// 位置最大值
// #define MIT_V_MIN -50		// 速度最小值
// #define MIT_V_MAX 50		// 速度最大值
// #define MIT_KP_MIN 0		// Kp最小值
// #define MIT_KP_MAX 500	// Kp最大值
// #define MIT_KD_MIN 0		// Kd最小值
// #define MIT_KD_MAX 5		// Kd最大值
// #define MIT_T_MIN -18		// 转矩最大值
// #define MIT_T_MAX 18		// 转矩最小值

// typedef enum
// {
// 	MIT_Motor_Mode = 0,
// 	MIT_Set_Zero_Position,
// 	MIT_Setup_Mode,
// 	MIT_Calibrate_Encoder,
// 	MIT_Display_Encoder,
// 	MIT_Exit_To_Menu,
// 	MIT_Get_Pos_Vel
// }MIT_Motor_Cmd_Index_t;
// extern const uint8_t MIT_Motor_Cmd[7][8];
// typedef struct
// {
// 	Can_Handle_t *can_handle; /**< 消息发送句柄 */
// 	uint16_t id;							/**< 发送数据包id */
// 	uint32_t timestamp;
// } MIT_Motor_tx_t;

// typedef struct 
// {
// 	Motor_t motor;
// 	uint8_t error_code;
// 	MIT_Motor_tx_t tx_config;
// 	Soft_WitchDog_t witchdog;
// }MIT_Motor_t;
// /**
//  * @brief 发送MIT电机指令
//  */
// #define MIT_Motor_Send_Cmd(MIT_motor,Cmd_Index) SEML_CAN_Send((MIT_motor)->tx_config.can_handle,(MIT_motor)->tx_config.id,(uint8_t*)MIT_Motor_Cmd[Cmd_Index],8,CAN_RTR_DATA)

// /**
//  * @brief MIT电机初始化
//  * 
//  * @param mit_motor mit电机句柄
//  * @param master_id 电机反馈id
//  * @param motor_id 电机接收id
//  * @param can_handle can句柄
//  * @return 电机句柄
//  */
// Motor_t *MIT_Motor_Init(MIT_Motor_t *mit_motor, uint16_t master_id, uint16_t motor_id, Can_Handle_t *can_handle);

// /**
//  * @brief  MIT电机控制
//  * @param  mit_motor  mit电机句柄
//  * @param  _pos   位置给定
//  * @param  _vel   速度给定
//  * @param  _KP    位置比例系数
//  * @param  _KD    位置微分系数
//  * @param  _torq  转矩给定值
//  */
// void MIT_Motor_Ctrl(MIT_Motor_t *mit_motor, float _pos, float _vel, float _KP, float _KD, float _torq);
// /**
//  * @brief MIT电机发送转矩
//  * @param mit_motor mit电机句柄
//  * @param data 转矩
//  */
// void MIT_Motor_Send_Torque(MIT_Motor_t *mit_motor, float data);
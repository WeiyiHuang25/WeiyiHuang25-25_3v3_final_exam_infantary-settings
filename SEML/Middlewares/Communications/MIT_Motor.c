// #include "MIT_Motor.h"
// const uint8_t MIT_Motor_Cmd[7][8] = 
// {
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfa},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf9},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfd},
// 	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01}
// };
// #define MIT_uint_to_float(x_int,x_min,x_max,bits) (((float)x_int) * (x_max - x_min) / ((float)((1 << bits) - 1)) + x_min)
// #define MIT_float_to_uint(x,x_min,x_max,bits) ((int)((x - x_min) * ((float)((1 << bits) - 1)) / (x_max - x_min)))
// /**
//  * @brief MIT电机的接收回调函数
//  * @param[out] dm_motor MIT电机句柄
//  * @param[in] message_pack 接收数据包
//  */
// void MIT_Motor_Callback(MIT_Motor_t *mit_motor, message_Pack_t const *const message_pack)
// {
// 	uint8_t *data, longth;
// 	static uint8_t id;
// 	uint16_t p_int, v_int, t_int;
// 	longth = Get_message_Pack_Data(message_pack, (void**)&data);
// 	// 检查长度是否正确
// 	if (longth != 8)
// 		return;
// 	// 检查id是否正确
// 	id = data[0] & 0x0f;
// 	if (id != (mit_motor->tx_config.id & 0x0f))
// 		return;
// 	// 开始解算
// 	mit_motor->error_code = data[0]>>4;
// 	// if(mit_motor->error_code!=0)
// 	// 	DM_Motor_Clear_Err(mit_motor);
// 	p_int = (data[1] << 8) | data[2];
// 	v_int = (data[3] << 4) | (data[4] >> 4);
// 	t_int = ((data[4] & 0xF) << 8) | data[5];
// 	mit_motor->motor.position = MIT_uint_to_float(p_int, MIT_P_MIN, MIT_P_MAX, 16);
// 	mit_motor->motor.speed = MIT_uint_to_float(v_int, MIT_V_MIN, MIT_V_MAX, 12);
// 	mit_motor->motor.torque = MIT_uint_to_float(t_int, MIT_T_MIN, MIT_T_MAX, 12);
// 	SEML_Feed_WitchDog(mit_motor);
// }
// /**
//  * @brief MIT电机初始化
//  * 
//  * @param mit_motor mit电机句柄
//  * @param master_id 电机反馈id
//  * @param motor_id 电机接收id
//  * @param can_handle can句柄
//  * @return 电机句柄
//  */
// Motor_t *MIT_Motor_Init(MIT_Motor_t *mit_motor, uint16_t master_id, uint16_t motor_id, Can_Handle_t *can_handle)
// {
// 	assert_param(mit_motor != NULL);
// 	// 订阅报文消息
// 	SEML_CAN_Rxmessage_Register(can_handle, master_id, (message_callback_fun_t)MIT_Motor_Callback, mit_motor);
// 	// 配置can句柄
// 	mit_motor->tx_config.can_handle = can_handle;
// 	mit_motor->tx_config.timestamp = 0;
// 	// 初始化电机类
// 	Motor_Init(&mit_motor->motor, (Motor_Send_Data_t)MIT_Motor_Send_Torque, mit_motor);
// 	mit_motor->tx_config.id = motor_id;
// }

// /**
//  * @brief  MIT电机控制
//  * @param  mit_motor  mit电机句柄
//  * @param  _pos   位置给定
//  * @param  _vel   速度给定
//  * @param  _KP    位置比例系数
//  * @param  _KD    位置微分系数
//  * @param  _torq  转矩给定值
//  */
// void MIT_Motor_Ctrl(MIT_Motor_t *mit_motor, float _pos, float _vel, float _KP, float _KD, float _torq)
// {
// 	uint8_t data[8];
// 	uint16_t pos_tmp, vel_tmp, kp_tmp, kd_tmp, tor_tmp;

// 	pos_tmp = MIT_float_to_uint(_pos, MIT_P_MIN, MIT_P_MAX, 16);
// 	vel_tmp = MIT_float_to_uint(_vel, MIT_V_MIN, MIT_V_MAX, 12);
// 	kp_tmp = MIT_float_to_uint(_KP, MIT_KP_MIN, MIT_KP_MAX, 12);
// 	kd_tmp = MIT_float_to_uint(_KD, MIT_KD_MIN, MIT_KD_MAX, 12);
// 	tor_tmp = MIT_float_to_uint(_torq, MIT_T_MIN, MIT_T_MAX, 12);

// 	data[0] = (pos_tmp >> 8);
// 	data[1] = pos_tmp;
// 	data[2] = (vel_tmp >> 4);
// 	data[3] = ((vel_tmp & 0xF) << 4) | (kp_tmp >> 8);
// 	data[4] = kp_tmp;
// 	data[5] = (kd_tmp >> 4);
// 	data[6] = ((kd_tmp & 0xF) << 4) | (tor_tmp >> 8);
// 	data[7] = tor_tmp;

// 	SEML_CAN_Send(mit_motor->tx_config.can_handle, mit_motor->tx_config.id, data, 8, CAN_RTR_DATA);
// }
// /**
//  * @brief MIT电机发送转矩
//  * @param mit_motor mit电机句柄
//  * @param data 转矩
//  */
// void MIT_Motor_Send_Torque(MIT_Motor_t *mit_motor, float data)
// {
// 	// 定时使能
// 	if (SEML_GetTick() - mit_motor->tx_config.timestamp > 500)
// 	{
// 		MIT_Motor_Send_Cmd(mit_motor,MIT_Motor_Mode);
// 		mit_motor->tx_config.timestamp = SEML_GetTick();
// 	}
// 	MIT_Motor_Ctrl(mit_motor, 0, 0, 0, 0, data);
// }
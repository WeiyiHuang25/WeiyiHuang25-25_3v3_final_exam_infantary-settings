
#include "DMPower.h"
/**
 * @brief  uint转float
 * @param  x_int   最大值
 * @param  x_min   最小值
 * @param  bits    位数
 */
static float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

/**
 * @brief  float转uint
 * @param  x_int   最大值
 * @param  x_min   最小值
 * @param  bits    位数
 */
static int float_to_uint(float x, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

/**
 * @brief  电机使能
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Enable(DM_Motor_t *dm_motor)
{
	uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief  电机失能
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Disable(DM_Motor_t *dm_motor)
{
	uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};
	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief  清除故障
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Clear_Err(DM_Motor_t *dm_motor)
{
	uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB};
	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief  电机保存零点
 * @param[in]  dm_motor  达妙电机句柄
 */
void DM_Motor_Save_Position_Zero(DM_Motor_t *dm_motor)
{
	uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief 达妙电机的接收回调函数
 * @param[out] dm_motor 达妙电机句柄
 * @param[in] message_pack 接收数据包
 */
void DM_Motor_Callback(DM_Motor_t *dm_motor, message_Pack_t const *const message_pack)
{
	uint8_t *data, longth,id;
	uint16_t p_int, v_int, t_int;
	longth = Get_message_Pack_Data(message_pack, (void**)&data);
	// 检查长度是否正确
	if (longth != 8)
		return;
	// 检查id是否正确
	id = data[0] & 0x0f;
	if (id != (dm_motor->tx_config.id & 0x0f))
		return;
	// 开始解算
	dm_motor->error_code = data[0]>>4;
	if(dm_motor->error_code!=0)
		DM_Motor_Clear_Err(dm_motor);
	p_int = (data[1] << 8) | data[2];
	v_int = (data[3] << 4) | (data[4] >> 4);
	t_int = ((data[4] & 0xF) << 8) | data[5];
	dm_motor->motor.position = uint_to_float(p_int, P_MIN, P_MAX, 16);
	dm_motor->motor.speed = uint_to_float(v_int, V_MIN, V_MAX, 12);
	dm_motor->motor.torque = uint_to_float(t_int, T_MIN, T_MAX, 12);
	SEML_Feed_WitchDog(dm_motor);
}
/**
 * @brief 达妙电机超时回调函数
 * @param dm_motor 达妙电机句柄
 */
void DM_Motor_Timeout_Callback(DM_Motor_t *dm_motor)
{
	dm_motor->motor.position = 0;
	dm_motor->motor.speed = 0;
	dm_motor->motor.torque = 0;
}
/**
 * @brief  MIT模式控下控制帧
 * @param  dm_motor   达妙电机句柄
 * @param  _pos   位置给定
 * @param  _vel   速度给定
 * @param  _KP    位置比例系数
 * @param  _KD    位置微分系数
 * @param  _torq  转矩给定值
 */
void DM_Motor_MIT_Ctrl(DM_Motor_t *dm_motor, float _pos, float _vel, float _KP, float _KD, float _torq)
{
	uint8_t data[8];
	uint16_t pos_tmp, vel_tmp, kp_tmp, kd_tmp, tor_tmp;

	pos_tmp = float_to_uint(_pos, P_MIN, P_MAX, 16);
	vel_tmp = float_to_uint(_vel, V_MIN, V_MAX, 12);
	kp_tmp = float_to_uint(_KP, KP_MIN, KP_MAX, 12);
	kd_tmp = float_to_uint(_KD, KD_MIN, KD_MAX, 12);
	tor_tmp = float_to_uint(_torq, T_MIN, T_MAX, 12);

	data[0] = (pos_tmp >> 8);
	data[1] = pos_tmp;
	data[2] = (vel_tmp >> 4);
	data[3] = ((vel_tmp & 0xF) << 4) | (kp_tmp >> 8);
	data[4] = kp_tmp;
	data[5] = (kd_tmp >> 4);
	data[6] = ((kd_tmp & 0xF) << 4) | (tor_tmp >> 8);
	data[7] = tor_tmp;

	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief  位置速度模式控下控制帧
 * @param  dm_motor  达妙电机句柄
 * @param  _pos   位置给定
 * @param  _vel   最大速度给定
 */
void DM_Motor_PosSpeed_Ctrl(DM_Motor_t *dm_motor, float _pos, float _vel)
{
	uint8_t data[8];
	uint8_t *pbuf, *vbuf;
	pbuf = (uint8_t *)&_pos;
	vbuf = (uint8_t *)&_vel;

	data[0] = *(pbuf + 0);
	data[1] = *(pbuf + 1);
	data[2] = *(pbuf + 2);
	data[3] = *(pbuf + 3);
	data[4] = *(vbuf + 0);
	data[5] = *(vbuf + 1);
	data[6] = *(vbuf + 2);
	data[7] = *(vbuf + 3);

	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief  速度模式控下控制帧
 * @param  dm_motor  达妙电机句柄
 * @param  _vel   速度给定
 */
void DM_Motor_Speed_Ctrl(DM_Motor_t *dm_motor, float _vel)
{
	uint8_t data[8];
	uint8_t *vbuf;
	vbuf = (uint8_t *)&_vel;

	data[0] = *(vbuf + 0);
	data[1] = *(vbuf + 1);
	data[2] = *(vbuf + 2);
	data[3] = *(vbuf + 3);
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;

	SEML_CAN_Send(dm_motor->tx_config.can_handle, dm_motor->tx_config.id, data, 8, CAN_RTR_DATA);
}

/**
 * @brief 达妙电机转矩控制
 * @param dm_motor 达妙电机句柄
 * @param data 转矩
 */
static void DM_Send_Torque(DM_Motor_t *dm_motor, float data)
{
	// 定时使能
	if (SEML_GetTick() - dm_motor->tx_config.timestamp > 1000)
	{
		DM_Motor_Enable(dm_motor);
		dm_motor->tx_config.timestamp = SEML_GetTick();
	}
	DM_Motor_MIT_Ctrl(dm_motor, 0, 0, 0, 0, data);
}

/**
 * @brief 达妙电机速度控制
 * @param dm_motor 达妙电机句柄
 * @param data 速度
 */
static void DM_Send_Speed(DM_Motor_t *dm_motor, float data)
{
	// 定时使能
	if (SEML_GetTick() - dm_motor->tx_config.timestamp > 1000)
	{
		DM_Motor_Enable(dm_motor);
		dm_motor->tx_config.timestamp = SEML_GetTick();
	}
	DM_Motor_Speed_Ctrl(dm_motor, data);
}

/**
 * @brief 达妙电机位置控制
 * @param dm_motor 达妙电机句柄
 * @param data 位置
 */
static void DM_Send_Position(DM_Motor_t *dm_motor, float data)
{
	// 定时使能
	if (SEML_GetTick() - dm_motor->tx_config.timestamp > 1000)
	{
		DM_Motor_Enable(dm_motor);
		dm_motor->tx_config.timestamp = SEML_GetTick();
	}
	DM_Motor_PosSpeed_Ctrl(dm_motor, data, dm_motor->max_speed);
}

/**
 * @brief 达妙电机初始化
 * @param dm_motor 达妙电机句柄
 * @param ctrl_mode 控制模式
 * @param master_id 电机反馈报文主id
 * @param motor_id 电机的控制报文id
 */
Motor_t *DM_Motor_Init(DM_Motor_t *dm_motor, DM_Motor_Ctrl_Mode_t ctrl_mode, uint16_t master_id, uint16_t motor_id, Can_Handle_t *can_handle, float max_speed)
{
	assert_param(dm_motor != NULL);
	SEML_CAN_Rxmessage_Register(can_handle, master_id, (message_callback_fun_t)DM_Motor_Callback, dm_motor);
	// 订阅报文消息
	// 配置can句柄
	dm_motor->tx_config.can_handle = can_handle;
	dm_motor->tx_config.timestamp = 0;
	// 注册看门狗
	SEML_WitchDog_Register(dm_motor, DJI_MOTOR_TIMEOUT, DM_Motor_Timeout_Callback);
	// 初始化电机
	switch (ctrl_mode)
	{
	case DM_Motor_MIT_Mode:
		Motor_Init(&dm_motor->motor, (Motor_Send_Data_t)DM_Send_Torque, dm_motor);
		dm_motor->tx_config.id = motor_id + 0x000;
		break;
	case DM_Motor_PosSpeed_Mode:
		Motor_Init(&dm_motor->motor, NULL, dm_motor);
		Motor_Position_Loop_Config(&dm_motor->motor, (Motor_Send_Data_t)DM_Send_Position, dm_motor);
		dm_motor->tx_config.id = motor_id + 0x100;
		dm_motor->max_speed = max_speed;
		break;
	case DM_Motor_Speed_Mode:
		Motor_Init(&dm_motor->motor, NULL, dm_motor);
		Motor_Speed_Loop_Config(&dm_motor->motor, (Motor_Send_Data_t)DM_Send_Speed, dm_motor);
		dm_motor->tx_config.id = motor_id + 0x200;
		break;
	default:
		assert_failed(__FILE__, __LINE__);
	}
	return &dm_motor->motor;
}

#include "vision.h"

extern Buzzer_t buzzer; 

/**
	* @brief 视觉初始化
	* @param[out] handle 视觉句柄
 	* @param[in] huart 视觉挂载的串口句柄
*/
void Vision_Init(Vision_t *handle, UART_IF_t *huart)
{
	memset(handle, 0x00, sizeof(Vision_t));
	handle->uart_status = SEML_TIMEOUT;
	SEML_UART_Init(&handle->uart, UART_Block_Mode, UART_DMA_Cycle_Mode, 100, huart);
	SEML_UART_Register_Callback(&handle->uart, Vision_Rx_Callback, handle);
	SEML_UART_Receive(&handle->uart, handle->Receive_by_vision.rx_buffer, Vision_Get_BufferSize);
#ifdef USE_SOFT_WITCHDOG
	 SEML_WitchDog_Register(handle, Vision_LOST_TIME, Vision_Timeout_Callback);
#endif
}


/**
	* @brief 发送数据给视觉数据处理函数
	* @param[out] handle 裁判系统句柄
 	* @param[in] huart 视觉挂载的串口句柄
*/
void Send_To_Vision_t(Reference_t *reference_data, Vision_t *handle)
{
	int temp;
	static uint8_t abuffer[Vision_Send_BufferSize]; // 发视觉数据
	//Yaw:-1800 ~ 1800,Pitch:-1800 ~ 1800,Roll:-900 ~ 900
    //发送格式：0xAF + Yaw(两个字节) + Pitch(两个字节) + Roll(两个字节) + 射速(两个字节) + 异或校验位(一个字节) + 0xFA
	handle->Send_to_vision.Send_Yaw = AHRS.euler_angle.yaw * 180 / PI * 10;
	handle->Send_to_vision.Send_Pitch = AHRS.euler_angle.pitch * 180 / PI * 10;
	handle->Send_to_vision.Send_Roll = AHRS.euler_angle.roll * 180 / PI * 10;
	// handle->Send_to_vision.Shoot_Speed = reference_data->shoot_data.bullet_speed; // 获取射速
	// 获取红蓝方RED :1 BLUE : 0
	if(reference_data->game_robot_status.robot_id > 10)
	 temp = 0;
	 else
	 temp = 1;
	handle->Send_to_vision.COLOUR = temp;			 
	handle->Send_to_vision.MODE = 0;
	//测试代码
	handle->Send_to_vision.Shoot_Speed = 27;
	// handle->Send_to_vision.COLOUR = 1;
	// handle->Send_to_vision.MODE = 1;
	// 抬升处理
	if (handle->Send_to_vision.Shoot_Speed == 0)
		handle->Send_to_vision.Shoot_Speed = 140;
	// 帧头
	abuffer[ID_FARME_HEADER] = 0xAF;
	// YAW
	abuffer[ID_YAW_HIGH] = handle->Send_to_vision.Send_Yaw >> 8;
	abuffer[ID_YAW_LOW] = handle->Send_to_vision.Send_Yaw & 0xFF;
	// PITCH
	abuffer[ID_PITCH_HIGH] = handle->Send_to_vision.Send_Pitch >> 8;
	abuffer[ID_PITCH_LOW] = handle->Send_to_vision.Send_Pitch & 0xFF;
	// ROLL
	abuffer[ID_ROLL_HIGH] = handle->Send_to_vision.Send_Roll >> 8;
	abuffer[ID_ROLL_LOW] = handle->Send_to_vision.Send_Roll & 0xFF;
	// 射速
	abuffer[ID_SHOOT_SPEED_HIGH] = handle->Send_to_vision.Shoot_Speed >> 8;
	abuffer[ID_SHOOT_SPEED_LOW] = handle->Send_to_vision.Shoot_Speed & 0xFF;
	// 颜色
	abuffer[ID_COLOUR] = handle->Send_to_vision.COLOUR;
	// 模式
	abuffer[ID_MODE] = handle->Send_to_vision.MODE;

	// 异或校验位
	handle->Send_to_vision.CheckSum = 0;
	for (int i = 1; i <= 10; i++)
		handle->Send_to_vision.CheckSum ^= abuffer[i];
	abuffer[ID_CHECK] = handle->Send_to_vision.CheckSum;
	// 帧尾
	abuffer[ID_END_OF_FARME] = 0xFA;

	SEML_UART_Transmit(&handle->uart, abuffer, Vision_Send_BufferSize);
}

/**
	* @brief 视觉接收回调函数
	* @param[in] uart 串口句柄
*/
void Vision_Rx_Callback(UART_Handle_t *uart)
{
	static uint32_t vision_tick = 0;
	vision_tick++;
	Vision_t *handle = SEML_UART_Get_Rx_Callback_Config(uart);
	SEML_Feed_WitchDog(handle);
	Vision_Rx_Handle(handle);
	handle->uart_status = Vision_Rx_Handle(handle);
}

/**
	* @brief 视觉超时回调函数
	* @param[out] handle 视觉句柄
*/
void Vision_Timeout_Callback(Vision_t *handle)
{
	SEML_UART_Abort(&handle->uart);
	SEML_UART_Receive(&handle->uart, handle->Receive_by_vision.rx_buffer, Vision_Get_BufferSize);
	handle->uart_status = SEML_TIMEOUT;
}

/**
	* @brief 解析收到的视觉数据
	* @param[in,out] handle 视觉句柄
*/
SEML_StatusTypeDef Vision_Rx_Handle(Vision_t *handle)
{
	static uint8_t buffer[Vision_Get_BufferSize];
	static uint8_t status = SEML_ERROR;
	uint8_t xor_Check = 0;
	uint16_t rx_len = SEML_UART_Get_Rx_Count(&handle->uart);
	// 没有接收到完整数据字节退出
	if(rx_len < Vision_Get_BufferSize)
		return status;
	memcpy(buffer, handle->Receive_by_vision.rx_buffer, Vision_Get_BufferSize);
	for (uint8_t i = 0; i < rx_len; i++)
	{
		if ((buffer[i] == 0xAF) && (buffer[(i + Vision_Get_BufferSize - 1) % Vision_Get_BufferSize] == 0xFA))
		{
			// 异或校验,校验结果和校验段相同异或结果为0
			for(uint8_t j = 1 ;j <= 8;j++)
				xor_Check ^= buffer[(i+j) % Vision_Get_BufferSize];
			if (xor_Check == 0)
			{
				Buzzer_Task_B_B(&buzzer, 100);
				handle->Receive_by_vision.Receive_Yaw = (buffer[(i + 1) % Vision_Get_BufferSize] << 8) | buffer[(i + 2) % Vision_Get_BufferSize];
				handle->Receive_by_vision.Receive_Pitch = (buffer[(i + 3) % Vision_Get_BufferSize] << 8) | buffer[(i + 4) % Vision_Get_BufferSize];
				handle->Receive_by_vision.Shoot_Mode = buffer[(i + 5) % Vision_Get_BufferSize];
				handle->Receive_by_vision.Shoot_Distance = (buffer[(i + 6) % Vision_Get_BufferSize] << 8) | buffer[(i + 7) % Vision_Get_BufferSize];
				status = SEML_OK;
				break;
			}
		}
	}
	handle->Receive_by_vision.status.data_validity = status;
	return status;
}

/**
	* @brief 视觉数据解析
*/
__weak void Vision_Control(void)
{
	int8_t Shoot_Mode;
	static SEML_StatusTypeDef status = SEML_ERROR;
	// 获取视觉数据
	status = Get_Vision_status(&vision);
	//注意视觉发过来的角度是否乘10，如果没有将 / 10去掉+
	vision.temp_yaw = Get_Vision_Yaw_Angle(&vision) / 10;
	vision.temp_pitch = Get_Vision_Pitch_Angle(&vision) / 10;
	// YAW零点突变处理
	if (vision.temp_yaw < -179)
		vision.temp_yaw = vision.temp_yaw + 360;
	else if (vision.temp_yaw > 179)
		vision.temp_yaw = vision.temp_yaw - 360;
		
	// Yaw解算，转换范围
	if (vision.temp_yaw > 0)
	{
		vision.Yaw_angle = vision.temp_yaw / 180.0f * PI;
		vision.Last_Time_Receive_Yaw = vision.Yaw_angle;
	}
	else if (vision.temp_yaw < 0)
	{
		vision.Yaw_angle = vision.temp_yaw / 180.0f * PI;
		vision.Last_Time_Receive_Yaw = vision.Yaw_angle;
	}

	// Pitch解算，转换范围
	vision.Pitch_angle = vision.temp_pitch / 180.0f * -PI; //注意视觉的角度，如果反过来加个负号
	vision.Last_Time_Receive_Pitch = vision.Pitch_angle;

	//角度限制
	if (vision.Yaw_angle <= -PI)
		vision.Yaw_angle = -PI;
	else if (vision.Yaw_angle >= PI)
		vision.Yaw_angle = PI;

	/* ********到这里对视觉接收数据处理完成******** */

	// 接收不到就不用视觉数据，用上一刻数据，

	if (status == SEML_ERROR || status == SEML_TIMEOUT)
	{
		vision.Yaw_angle = AHRS.euler_angle.yaw;
		vision.Pitch_angle = AHRS.euler_angle.pitch;
	}
	//斜坡函数处理
	Robo_Get_Message_Cmd("Set_Gimbal_Yaw_Angle", vision.Solved_Yaw);
	vision.Solved_Yaw = Ramp_float(vision.Yaw_angle, vision.Solved_Yaw, Vision_Step);
	Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", vision.Solved_Yaw);

	//斜坡函数处理
	Robo_Get_Message_Cmd("Set_Gimbal_Pitch_Angle", vision.Solved_Pitch);
	vision.Solved_Pitch = Ramp_float(vision.Pitch_angle, vision.Solved_Pitch, Vision_Step);
	/* ********到这里期望值处理完成******** */
	// pitch期望控制角度限幅
	if (vision.Solved_Pitch > 20  / 360.0f * 2 * PI)
		vision.Solved_Pitch = 20 / 360.0f * 2 * PI;
	else if (vision.Solved_Pitch <= -30 / 360.0f * 2 * PI)
		vision.Solved_Pitch = -30 / 360.0f * 2 * PI;
	Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", vision.Solved_Pitch);
}



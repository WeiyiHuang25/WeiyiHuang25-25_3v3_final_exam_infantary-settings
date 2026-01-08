#include "reference.h"
ext_robot_status_t *ext_game_robot_status;
/**
 * @brief 裁判系统初始化
 * @param[out] handle 裁判系统句柄
 * @param[in] huart 裁判系统挂载的串口句柄
 */
void Reference_Init(Reference_t *handle, UART_IF_t *huart)
{
	// 初始化裁判系统数据位
	memset(handle, 0x00, sizeof(Reference_t));
	handle->config.status.data_validity = SEML_TIMEOUT;
	// 初始化串口
	SEML_UART_Init(&handle->config.uart, UART_Block_Mode, UART_DMA_Cycle_Mode, 0, huart);
	SEML_UART_Register_Callback(&handle->config.uart, Reference_Rx_Callback, handle);
	SEML_UART_Receive(&handle->config.uart, handle->config.rx_buffer, MAXSIZE);
#ifdef USE_SOFT_WITCHDOG 
	SEML_WitchDog_Register(handle, JUDGE_LOST_TIME, Reference_Timeout_Callback);
#endif
	ext_game_robot_status = &handle->game_robot_status;
}
/**
 * @brief 裁判系统接收回调函数
 * @param[in] uart 串口句柄
 */
void Reference_Rx_Callback(UART_Handle_t *uart)
{
	static uint32_t reference_tick = 0;
	reference_tick++;
	Reference_t *handle = SEML_UART_Get_Rx_Callback_Config(uart);
	SEML_Feed_WitchDog(handle);
	Reference_Rx_Handle(handle);
}

/**
 * @brief 裁判系统超时回调函数
 * @param[in] handle 串口句柄
 */
void Reference_Timeout_Callback(Reference_t *handle)
{
	handle->config.status.data_validity = SEML_TIMEOUT;
	SEML_UART_Abort(&handle->config.uart);
	SEML_UART_Receive(&handle->config.uart, handle->config.rx_buffer, MAXSIZE);
}

static const uint8_t ref_crc8_init = 0xff;
static const uint8_t ref_crc8_tab[256] =
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

static uint8_t ref_get_crc8(uint8_t *p_msg, uint32_t len, uint8_t crc8)
{
	uint8_t uc_index;

	while (len--)
	{
		uc_index = crc8 ^ (*p_msg++);
		crc8 = ref_crc8_tab[uc_index];
	}

	return (crc8);
}

uint32_t ref_verify_crc8(uint8_t *p_msg, uint32_t len)
{
	uint8_t uc_expected = 0;

	if ((p_msg == 0) || (len <= 2))
	{
		return 0;
	}

	uc_expected = ref_get_crc8(p_msg, len - 1, ref_crc8_init);

	return (uc_expected == p_msg[len - 1]);
}

void ref_append_crc8(uint8_t *p_msg, uint32_t len)
{
	uint8_t crc8 = 0;

	if ((p_msg == 0) || (len <= 2))
	{
		return;
	}

	crc8 = ref_get_crc8((uint8_t *)p_msg, len - 1, ref_crc8_init);

	p_msg[len - 1] = crc8;
}

static uint16_t ref_crc16_init = 0xffff;
static const uint16_t ref_crc16_tab[256] =
		{
				0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
				0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
				0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
				0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
				0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
				0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
				0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
				0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
				0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
				0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
				0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
				0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
				0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
				0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
				0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
				0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
				0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
				0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
				0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
				0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
				0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
				0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
				0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
				0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
				0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
				0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
				0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
				0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
				0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
				0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
				0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
				0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78};

/*
**  Descriptions: CRC16 checksum function
**  Input:        Data to check,Stream length, initialized checksum
**  Output:       CRC checksum
*/
uint16_t ref_get_crc16(uint8_t *p_msg, uint16_t len, uint16_t crc16)
{
	uint8_t data;

	if (p_msg == NULL)
	{
		return 0xFFFF;
	}

	while (len--)
	{
		data = *p_msg++;
		(crc16) = ((uint16_t)(crc16) >> 8) ^ ref_crc16_tab[((uint16_t)(crc16) ^ (uint16_t)(data)) & 0x00ff];
	}

	return crc16;
}

/*
**  Descriptions: CRC16 Verify function
**  Input:        Data to Verify,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
uint32_t ref_verify_crc16(uint8_t *p_msg, uint32_t len)
{
	uint16_t w_expected = 0;

	if ((p_msg == NULL) || (len <= 2))
	{
		return 1;
	}
	w_expected = ref_get_crc16(p_msg, len - 2, ref_crc16_init);

	return ((w_expected & 0xff) == p_msg[len - 2] && ((w_expected >> 8) & 0xff) == p_msg[len - 1]);
}

/*
**  Descriptions: append CRC16 to the end of data
**  Input:        Data to CRC and append,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
void ref_append_crc16(uint8_t *p_msg, uint16_t len)
{
	uint16_t crc16 = 0;

	if ((p_msg == NULL) || (len <= 2))
	{
		return;
	}
	crc16 = ref_get_crc16((uint8_t *)p_msg, len - 2, ref_crc16_init);

	p_msg[len - 2] = (uint8_t)(crc16 & 0x00ff);
	p_msg[len - 1] = (uint8_t)((crc16 >> 8) & 0x00ff);
}

/**
 * @brief  解析接收到的裁判系统信息
 * @param[in,out] handle 裁判系统句柄
 */
SEML_StatusTypeDef Reference_Rx_Handle(Reference_t *handle)
{
	static uint8_t buffer[MAXSIZE];
	SEML_StatusTypeDef status = SEML_ERROR;
	uint16_t data_len = 0, rx_len = SEML_UART_Get_Rx_Count(&handle->config.uart);
	int16_t CmdID;
	// 拷贝一份，避免在解算过程中继续接收到数据
	memcpy(buffer, handle->config.rx_buffer, MAXSIZE);
	for (int i = 0; i < rx_len - 9; i++) // 读队列数据，直到读完
	{
		if (buffer[i] != 0xA5 || ref_verify_crc8(&buffer[i], 5) != 1) // 帧头解析无效,读取下一位
			continue;
		data_len = (uint16_t)buffer[i + 1] | (uint16_t)buffer[i + 2] << 8;
		if (ref_verify_crc16(&buffer[i], 9 + data_len) != 1) // 整包校验无效,读取下一位
			continue;
		status = SEML_OK;
		CmdID = (buffer[i + 6] << 8 | buffer[i + 5]);
		switch (CmdID)
		{
		// 比赛状态数据
		case cmd_game_status:
			memcpy(&handle->game_status, &buffer[i + 7], LEN_game_status);
			break;
		// 比赛结果
		case cmd_game_result:
			memcpy(&handle->game_robot_HP, &buffer[i + 7], LEM_game_result);
			break;
		// 比赛机器人血量数据
		case cmd_game_robot_HP:
			memcpy(&handle->game_robot_HP, &buffer[i + 7], LEN_game_robot_HP);
			break;
		// 场地事件数据
		case cmd_event_data:
			memcpy(&handle->event_data, &buffer[i + 7], LEN_event_data);
			break;
		// 补给站动作标识
		case cmd_supply_projectile_action:
			memcpy(&handle->supply_projectile_action, &buffer[i + 7], LEN_supply_projectile_action);
			break;
		// 裁判警告信息
		case cmd_referee_warning:
			memcpy(&handle->referee_warning, &buffer[i + 7], LEN_referee_warning);
			break;
		// 飞镖发射口倒计时
		case cmd_dart_remaining_time:
			memcpy(&handle->dart_remaining_time, &buffer[i + 7], LEN_dart_remaining_time);
			break;
		// 机器人状态数据
		case cmd_game_robot_status:
			memcpy(&handle->game_robot_status, &buffer[i + 7], LEN_game_robot_state);
			break;
		// 实时功率热量数据
		case cmd_power_heat_data:
			memcpy(&handle->power_heat_data, &buffer[i + 7], LEN_power_heat_data);
			break;
		// 机器人位置数据
		case cmd_game_robot_pos:
			memcpy(&handle->game_robot_pos, &buffer[i + 7], LEN_game_robot_pos);
			break;
		// 机器人增益数据
		case cmd_buff:
			memcpy(&handle->buff, &buffer[i + 7], LEN_buff_musk);
			break;
		// 空中机器人能量状态数据
		case cmd_aerial_robot_energy:
			memcpy(&handle->aerial_robot_energy, &buffer[i + 7], LEN_aerial_robot_energy);
			break;
		// 伤害状态数据
		case cmd_robot_hurt:
			memcpy(&handle->robot_hurt, &buffer[i + 7], LEN_robot_hurt);
			break;
		// 实时射击数据
		case cmd_shoot_data:
			memcpy(&handle->shoot_data, &buffer[i + 7], LEN_shoot_data);
			break;
		// 子弹剩余发送数，空中机器人以及哨兵机器人发送
		case cmd_bullet_remaining:
			memcpy(&handle->bullet_remaining, &buffer[i + 7], LEN_bullet_remaining);
			break;
		// 机器人 RFID 状态
		case cmd_rfid_status:
			memcpy(&handle->rfid_status, &buffer[i + 7], LEN_rfid_status);
			break;
		// 飞镖机器人客户端指令数据
		case cmd_dart_client_cmd:
			memcpy(&handle->dart_client_cmd, &buffer[i + 7], LEN_dart_client_cmd);
			break;
		// 机器人间交互数据
		case cmd_student_interactive_header_data:
			memcpy(&handle->student_interactive_header_data, &buffer[i + 7], LEN_student_interactive_header_data);
			break;
		// 客户端小地图交互数据
		case cmd_robot_command:
			memcpy(&handle->game_status, &buffer[i + 7], LEN_robot_command);
			break;
		// 键盘、鼠标信息，通过图传串口发送
		case cmd_ext_picture_command:
			memcpy(&handle->picture_command, &buffer[i + 7], LEN_picture_command);
			break;
		// 客户端小地图接收雷达信息
		case cmd_client_map_command:
			memcpy(&handle->client_map_command, &buffer[i + 7], LEN_client_map_command);
			break;
		case cmd_custom_info:
			memcpy(&handle->ui_command, &buffer[i + 7], LEN_custom_info);
		}	
	}
	handle->config.status.data_validity = status;
	// 解算匹配置OK
	if (status == SEML_OK)
		return SEML_OK;
	return SEML_ERROR;
}

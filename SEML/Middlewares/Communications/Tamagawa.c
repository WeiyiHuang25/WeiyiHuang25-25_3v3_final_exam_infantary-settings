#include "Tamagawa.h"
static inline uint16_t Tamagawa_Get_Rx_Longth(TAMAGAWA_Cmd_t cmd)
{
	switch (cmd)
	{
	case TAMAGAWA_DATA_ID2:
		return 4;
	case TAMAGAWA_DATA_ID0:
	case TAMAGAWA_DATA_ID1:
	case TAMAGAWA_DATA_ID7:
	case TAMAGAWA_DATA_ID8:
	case TAMAGAWA_DATA_IDC:
		return 6;
	case TAMAGAWA_DATA_ID3:
		return 11;
	default:
		assert_param(0);
	}
}
/**
 * @brief 多摩川编码器初始化
 * @param handle 多摩川编码器句柄
 * @param huart 串口接口句柄
 * @param tx_en_part 458发送使能端口
 * @param tx_en_pin 458发送使能引脚
 */
void Tamagawa_Init(Tamagawa_Encode_t *handle, UART_IF_t *huart, GPIO_IF_t *tx_en_part, uint16_t tx_en_pin)
{
	handle->ALMC = 0;
	handle->control_field = 0;
	handle->cycles = 0;
	handle->ENID = 0;
	handle->position = 0;
	SEML_RS_485_Init(&handle->uart_handle, UART_Block_Mode, UART_DMA_Cycle_Mode, 1, huart, tx_en_part, tx_en_pin);
	SEML_UART_Register_Callback(&handle->uart_handle, Tamagawa_callback, handle);
}

/**
 * @brief 向多摩川编码器发送指令
 * @param handle 多摩川编码器句柄
 * @param cmd 指令
 */
void Tamagawa_Send_Cmd(Tamagawa_Encode_t *handle, TAMAGAWA_Cmd_t cmd)
{
	uint8_t rx_longth = Tamagawa_Get_Rx_Longth(cmd);
	handle->control_field = cmd;

	SEML_UART_Transmit(&handle->uart_handle, &cmd, 1);
	SEML_UART_Receive(&handle->uart_handle, handle->rx_buffer, rx_longth);
}

/**
 * @brief 多摩川编码器接收回调函数
 * @param uart 串口句柄
 */
void Tamagawa_callback(UART_Handle_t *uart)
{
	Tamagawa_Encode_t *handle = SEML_UART_Get_Rx_Callback_Config(uart);
	uint16_t rx_longth = Tamagawa_Get_Rx_Longth(handle->control_field);
	// CRC多项式:x^8 + 1
	uint16_t crc = 0;
	for (uint16_t i = 0; i < rx_longth + 3; i++)
		crc = handle->rx_buffer[i];
	// 校验结果正确
	if (crc != 0)
		return;
	// SF位检查

	// DF解算
	switch (handle->control_field)
	{
	case TAMAGAWA_DATA_ID3:
		handle->cycles = (uint32_t)handle->rx_buffer[8] << 16 + handle->rx_buffer[7] << 8 + handle->rx_buffer[6];
		handle->ALMC = handle->rx_buffer[9];
		handle->ENID = handle->rx_buffer[5];
	case TAMAGAWA_DATA_ID0:
	case TAMAGAWA_DATA_ID7:
	case TAMAGAWA_DATA_ID8:
	case TAMAGAWA_DATA_IDC:
		handle->position = handle->rx_buffer[4] << 16 | handle->rx_buffer[3] << 8 | handle->rx_buffer[2];
		break;
	case TAMAGAWA_DATA_ID1:
		handle->cycles = (uint32_t)handle->rx_buffer[4] << 16 + handle->rx_buffer[3] << 8 + handle->rx_buffer[2];
		break;
	case TAMAGAWA_DATA_ID2:
		handle->ENID = handle->rx_buffer[2];
		break;
	}
	handle->control_field = 0;
}
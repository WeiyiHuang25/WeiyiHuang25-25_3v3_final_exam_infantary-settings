#include "uart_if.h"
#if USE_HAL_DRIVER
#define __SEML_UART_Block_Transmit(uart, data, longth) HAL_UART_Transmit((uart)->huart, data, longth, (uart)->timeout)
#define __SEML_UART_DMA_Transmit(uart, data, longth) HAL_UART_Transmit_DMA((uart)->huart, data, longth)
#define __SEML_UART_IT_Transmit(uart, data, longth) HAL_UART_Transmit_IT((uart)->huart, data, longth)
#define __SEML_UART_Block_Receive(uart, data, longth) HAL_UART_Receive((uart)->huart, data, longth, (uart)->timeout)
#define __SEML_UART_DMA_Receive(uart, data, longth) HAL_UART_Receive_DMA((uart)->huart, data, longth)
#define __SEML_UART_IT_Receive(uart, data, longth) HAL_UART_Receive_IT((uart)->huart, data, longth)
#define __SEML_UART_DMA_ReceiveToIdle(uart, data, longth) HAL_UARTEx_ReceiveToIdle_DMA((uart)->huart, data, longth)
#define __SEML_UART_DMA_Stop(uart) HAL_UART_DMAStop(uart->huart)
#define __SEML_UART_IT_Stop(uart) HAL_UART_Abort_IT(uart->huart)
#define __SEML_UART_Block_Stop(uart) HAL_UART_Abort(uart->huart)
// 使用循环模式的dma接收在接收完毕后会自动装载值,当传输值和dma值相等时候认为传输完成
#define __SEML_UART_Get_Rx_Count(uart) (uart->huart->RxXferSize == __HAL_DMA_GET_COUNTER(uart->huart->hdmarx) ? uart->huart->RxXferSize : (uart->huart->RxXferSize - __HAL_DMA_GET_COUNTER(uart->huart->hdmarx)))
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
#define __SEML_UART_MSP_Init(uart)                                                                                               \
	do                                                                                                                             \
	{                                                                                                                              \
		HAL_UART_RegisterRxEventCallback((uart)->huart, (pUART_RxEventCallbackTypeDef)SEML_UARTEx_RxEventCallback);                  \
		HAL_UART_RegisterCallback((uart)->huart, HAL_UART_RX_COMPLETE_CB_ID, (pUART_CallbackTypeDef)SEML_UART_Rx_Complete_Callback); \
		HAL_UART_RegisterCallback((uart)->huart, HAL_UART_TX_COMPLETE_CB_ID, (pUART_CallbackTypeDef)SEML_UART_Tx_Complete_Callback); \
	} while (0)
#else
#define __SEML_UART_MSP_Init(uart)
#endif
#else
#error Error: uart interface is not configured(uart_if.c)
#endif
UART_Handle_t *uart_handle_root;
/**
 * @brief 串口初始化
 * @param uart 串口句柄
 * @param tx_mode 发送模式
 * @param rx_mode 接收模式
 * @param timeout 超时时间
 * @param huart 串口接口句柄
 */
void SEML_UART_Init(UART_Handle_t *uart, UART_Transmission_Mode_t tx_mode, UART_Transmission_Mode_t rx_mode, uint32_t timeout, UART_IF_t *huart)
{
	assert_param(uart != NULL);
	UART_Handle_t *temp_handle = uart_handle_root;
	uart->huart = huart;
	uart->type = UART_Type_TTL;
	uart->tx_mode = tx_mode;
	uart->rx_mode = rx_mode;
	uart->timeout = timeout;
	uart->next = NULL;
	// 根节点未初始化进行添加
	if (uart_handle_root == NULL)
		uart_handle_root = uart;
	else
	{
		while (temp_handle->next != NULL)
		{
			// 链表中查找到不再添加
			if (temp_handle == uart)
				assert_param(0);
			temp_handle = temp_handle->next;
		}
		temp_handle->next = uart;
	}
	__SEML_UART_MSP_Init(uart);
}

/**
 * @brief 485模式串口初始化
 * @param uart 串口句柄
 * @param tx_mode 发送模式
 * @param rx_mode 接收模式
 * @param timeout 超时时间
 * @param huart 串口接口句柄
 * @param tx_en_part 发送使能端口
 * @param tx_en_pin 发送使能引脚
 */
void SEML_RS_485_Init(UART_Handle_t *uart, UART_Transmission_Mode_t tx_mode, UART_Transmission_Mode_t rx_mode, uint32_t timeout, UART_IF_t *huart, GPIO_IF_t *tx_en_part, uint16_t tx_en_pin)
{
	assert_param(uart != NULL);
	SEML_UART_Init(uart, tx_mode, rx_mode, timeout, huart);
	SEML_GPIO_Pin_Register(&uart->tx_en_gpio, tx_en_part, tx_en_pin);
	uart->type = UART_Type_RS485;
}

/**
 * @brief 注册串口接收回调函数
 * @param uart 串口句柄
 * @param rx_callback_fun 接收回调函数
 * @param rx_callback_config 接收回调函数配置
 */
void SEML_UART_Register_Callback(UART_Handle_t *uart, Rx_Callback_t rx_callback_fun, void *rx_callback_config)
{
	assert_param(uart != NULL);
	uart->rx_callback_fun = (void(*)(void *))rx_callback_fun;
	uart->rx_callback_config = rx_callback_config;
}

/**
 * @brief 串口发送函数
 * @param uart 串口句柄
 * @param data 数据指针
 * @param longth 数据长度
 * @return 函数运行状态
 */
SEML_StatusTypeDef SEML_UART_Transmit(UART_Handle_t *uart, uint8_t *data, uint8_t longth)
{
	uint8_t status;
	assert_param(uart != NULL);
	if (data == 0 || longth == 0)
		return SEML_ERROR;
	if (uart->type == UART_Type_RS485)
		SEML_GPIO_Pin_Set(&uart->tx_en_gpio);
	uart->tx_status = UART_Run;
	switch (uart->tx_mode)
	{
	case UART_Block_Mode:
		status = __SEML_UART_Block_Transmit(uart, data, longth);
		if (uart->type == UART_Type_RS485)
			SEML_GPIO_Pin_Reset(&uart->tx_en_gpio);
		uart->tx_status = UART_Stop;
		break;
	case UART_DMA_Mode:
		status = __SEML_UART_DMA_Transmit(uart, data, longth);
		break;
	case UART_IT_Mode:
		status = __SEML_UART_IT_Transmit(uart, data, longth);
		break;
	default:
		assert_param(0);
	}
	return (SEML_StatusTypeDef)status;
}

/**
 * @brief 串口接收函数
 * @param uart 串口句柄
 * @param data 数据指针
 * @param longth 数据长度
 * @return 函数运行状态
 */
SEML_StatusTypeDef SEML_UART_Receive(UART_Handle_t *uart, uint8_t *data, uint16_t longth)
{
	assert_param(uart != NULL);
	uint8_t status;
	if (data == 0 || longth == 0)
		return SEML_ERROR;
	uart->rx_buffer = data;
	uart->rx_buffer_longth = longth;
	uart->rx_status = UART_Run;
	uart->rx_start_tick = SEML_GetTick();
	switch (uart->rx_mode)
	{
	case UART_Block_Mode:
		status = __SEML_UART_Block_Receive(uart, data, longth);
		uart->rx_status = UART_Stop;
		if (status == SEML_OK && uart->rx_callback_fun != NULL)
			uart->rx_callback_fun(uart);
		break;
	case UART_DMA_Mode:
	case UART_DMA_Cycle_Mode:
		status = __SEML_UART_DMA_ReceiveToIdle(uart, data, longth);
		break;
	case UART_IT_Mode:
	case UART_IT_Cycle_Mode:
		status = __SEML_UART_IT_Receive(uart, data, longth);
		break;
	default:
		assert_param(0);
	}
	return (SEML_StatusTypeDef)status;
}

/**
 * @brief 串口接收完成中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UART_Rx_Complete_Callback(UART_IF_t *huart)
{
	assert_param(huart != NULL);
	UART_Handle_t *handle = uart_handle_root;
	while (handle != NULL)
	{
		if (handle->huart == huart)
		{
			handle->rx_count = handle->rx_buffer_longth;
			switch (handle->rx_mode)
			{
			case UART_Block_Mode:
				break;
			case UART_DMA_Mode:
				__SEML_UART_DMA_Stop(handle);
				handle->rx_status = UART_Stop;
			case UART_IT_Mode:
				if (handle->rx_callback_fun != NULL)
					handle->rx_callback_fun(handle);
				handle->rx_status = UART_Stop;
				break;
			case UART_DMA_Cycle_Mode:
				__SEML_UART_DMA_Stop(handle);
				if (handle->rx_callback_fun != NULL)
					handle->rx_callback_fun(handle);
				if (handle->rx_status == UART_Run)
				{
					handle->rx_start_tick = SEML_GetTick();
					__SEML_UART_DMA_Receive(handle, handle->rx_buffer, handle->rx_buffer_longth);
				}
				break;
			case UART_IT_Cycle_Mode:
				if (handle->rx_callback_fun != NULL)
					handle->rx_callback_fun(handle);
				if (handle->rx_status == UART_Run)
				{
					handle->rx_start_tick = SEML_GetTick();
					__SEML_UART_IT_Receive(handle, handle->rx_buffer, handle->rx_buffer_longth);
				}
				break;
			default:
				assert_param(0);
			}
			handle->rx_count = 0;
			return;
		}
		handle = handle->next;
	}
}

/**
 * @brief 串口接收空闲中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UARTEx_RxEventCallback(UART_IF_t *huart)
{
	assert_param(huart != NULL);
	UART_Handle_t *handle = uart_handle_root;
	while (handle != NULL)
	{
		if (handle->huart == huart)
		{

			__SEML_UART_DMA_Stop(handle);
			// 若接收数量未满预期接收值,未超过超时时间则不复位继续接收
			handle->rx_count += __SEML_UART_Get_Rx_Count(handle);
			if (handle->rx_callback_fun != NULL)
				handle->rx_callback_fun(handle);
			if (handle->rx_count >= handle->rx_buffer_longth || handle->rx_start_tick + handle->timeout <= SEML_GetTick())
				handle->rx_count = 0;
			if (handle->rx_status == UART_Run)
			{
				handle->rx_start_tick = SEML_GetTick();
				__SEML_UART_DMA_ReceiveToIdle(handle, &handle->rx_buffer[handle->rx_count], handle->rx_buffer_longth - handle->rx_count);
			}
			return;
		}
		handle = handle->next;
	}
}

/**
 * @brief 串口发送完成中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UART_Tx_Complete_Callback(UART_IF_t *huart)
{
	assert_param(huart != NULL);
	UART_Handle_t *handle = uart_handle_root;
	while (handle != NULL)
	{
		if (handle->huart == huart)
		{
			handle->tx_status = UART_Stop;
			// 如果是485模式关闭串口发送
			if (handle->type == UART_Type_RS485)
				SEML_GPIO_Pin_Reset(&handle->tx_en_gpio);
			switch (handle->tx_mode)
			{
			case UART_Block_Mode:
				break;
			case UART_DMA_Mode:
				__SEML_UART_DMA_Stop(handle);
				break;
			case UART_IT_Mode:
				break;
			default:
				assert_param(0);
			}
			return;
		}
		handle = handle->next;
	}
}

/**
 * @brief 串口通讯终止函数
 * @param uart 串口句柄
 */
void SEML_UART_Abort(UART_Handle_t *uart)
{
	assert_param(uart != NULL);
	uart->rx_status = UART_Stop;
	switch (uart->rx_mode)
	{
	case UART_Block_Mode:
		__SEML_UART_Block_Stop(uart);
		break;
	case UART_DMA_Mode:
	case UART_DMA_Cycle_Mode:
		__SEML_UART_DMA_Stop(uart);
		break;
	case UART_IT_Mode:
	case UART_IT_Cycle_Mode:
		__SEML_UART_IT_Stop(uart);
		break;
	default:
		assert_param(0);
	}
}

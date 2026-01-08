#include "DR16_Remote.h"

#define KEY_Ctrl RC_Ctrl
#define MOUSE_Ctrl RC_Ctrl
uint8_t Get_Key_Status(void *config)
{
	RC_Key_t key = (RC_Key_t)config;
	return DR16_Get_Key_Status(&KEY_Ctrl,key);
}

uint8_t Get_Mouse_Left_Status(void *config)
{
	RC_Key_t mouse = (RC_Key_t)config;
	return DR16_Get_Mouse_Left_Status(&MOUSE_Ctrl);
}

uint8_t Get_Mouse_Right_Status(void *config)
{
	RC_Key_t mouse = (RC_Key_t)config;
	return DR16_Get_Mouse_Right_Status(&MOUSE_Ctrl);
}

RC_Ctrl_t RC_Ctrl =
		{
				.rc.ch0 = 1024,
				.rc.ch1 = 1024,
				.rc.ch2 = 1024,
				.rc.ch3 = 1024,
				.rc.s1 = RC_S_DOWM,
				.rc.s2 = RC_S_DOWM,
				.rc.sw = 1024,
};
int32_t dead_zone(int32_t input)
{
	if (input > DR16_RC_OFFSET + DR16_DEAD_ZONE)
		return input;
	if (input < DR16_RC_OFFSET - DR16_DEAD_ZONE)
		return input;
	return DR16_RC_OFFSET;
}
/**
 * @brief DR16遥控器初始化
 * @param huart 串口句柄
 */
void DR16_Init(RC_Ctrl_t *handle, UART_IF_t *huart)
{
	// 句柄复位为初始值
	handle->rx_tick = 0;
	handle->rc.ch0 = 1024;
	handle->rc.ch1 = 1024;
	handle->rc.ch2 = 1024;
	handle->rc.ch3 = 1024;
	handle->rc.sw = 1024;
	handle->rc.s1 = RC_S_DOWM;
	handle->rc.s2 = RC_S_DOWM;

	handle->mouse.x = 0;
	handle->mouse.y = 0;
	handle->mouse.z = 0;
	handle->mouse.press_l = 0;
	handle->mouse.press_r = 0;
	handle->status = SEML_OK;
	// 初始化串口
	SEML_UART_Init(&handle->uart, UART_Block_Mode, UART_DMA_Cycle_Mode, DR16_TIMEOUT, huart);
	SEML_UART_Register_Callback(&handle->uart, DR16_Rx_Callback, handle);
	SEML_UART_Receive(&handle->uart, handle->buffer, 18);

#ifdef USE_SOFT_WITCHDOG 
	// 注册软件看门狗
	SEML_WitchDog_Register(handle, DR16_TIMEOUT, DR16_Rx_Timeout_Reset_Callack);
#endif
}
#define IS_DR16_CHANNEL_DATA(_data) ((_data) >= 364 && (_data) <= 1684)
#define IS_DR16_SWITCH_DATA(_data) ((_data) == RC_S_UP || (_data) == RC_S_MID || (_data) == RC_S_DOWM)
/**
 * @brief DR16接收回调函数
 * @param config 接收句柄
 */
void DR16_Rx_Callback(UART_Handle_t *config)
{
	RC_Ctrl_t *handle = SEML_UART_Get_Rx_Callback_Config(config);
	if(SEML_UART_Get_Rx_Count(config) != 18)
		return;
	do
	{
		// 解算接收到的数据
		handle->rc.ch0 = (handle->buffer[0] | (handle->buffer[1] << 8)) & 0x07ff;
		if (!IS_DR16_CHANNEL_DATA(handle->rc.ch0))
			break;
		handle->rc.ch1 = ((handle->buffer[1] >> 3) | (handle->buffer[2] << 5)) & 0x07ff;
		if (!IS_DR16_CHANNEL_DATA(handle->rc.ch1))
			break;
		handle->rc.ch2 = ((handle->buffer[2] >> 6) | (handle->buffer[3] << 2) | (handle->buffer[4] << 10)) & 0x07ff;
		if (!IS_DR16_CHANNEL_DATA(handle->rc.ch2))
			break;
		handle->rc.ch3 = ((handle->buffer[4] >> 1) | (handle->buffer[5] << 7)) & 0x07ff;
		if (!IS_DR16_CHANNEL_DATA(handle->rc.ch3))
			break;
		handle->rc.sw = ((handle->buffer[16]) | (handle->buffer[17] << 8)) & 0x07ff;
		if (!IS_DR16_CHANNEL_DATA(handle->rc.sw))
			break;
		handle->rc.s1 = (RC_S_t)(((handle->buffer[5] >> 4) & 0x000C) >> 2);
		if (!IS_DR16_SWITCH_DATA(handle->rc.s1))
			break;
		handle->rc.s2 = (RC_S_t)((handle->buffer[5] >> 4) & 0x0003);
		if (!IS_DR16_SWITCH_DATA(handle->rc.s2))
			break;
		handle->mouse.x = handle->buffer[6] | (handle->buffer[7] << 8);
		handle->mouse.y = handle->buffer[8] | (handle->buffer[9] << 8);
		handle->mouse.z = handle->buffer[10] | (handle->buffer[11] << 8);
		handle->mouse.press_l = handle->buffer[12];
		handle->mouse.press_r = handle->buffer[13];

		*(uint16_t *)&handle->keyboard = handle->buffer[14] | (handle->buffer[15] << 8);
#ifdef USE_SOFT_WITCHDOG
	// 喂狗
	SEML_Feed_WitchDog(handle);
#endif
		handle->rx_tick = DR16_GetTick();
		handle->status = SEML_OK;
		return;
	} while (0);
	// 接收数据错误,置标志位,遥控器复位
	handle->status = SEML_ERROR;
	DR16_Reset(handle);
}
/**
 * @brief DR16接收数据复位
 */
void DR16_Reset(RC_Ctrl_t *handle)
{
	handle->rx_tick = 0;
	handle->rc.ch0 = 1024;
	handle->rc.ch1 = 1024;
	handle->rc.ch2 = 1024;
	handle->rc.ch3 = 1024;
	handle->rc.sw = 1024;
	handle->rc.s1 = RC_S_DOWM;
	handle->rc.s2 = RC_S_DOWM;

	handle->mouse.x = 0;
	handle->mouse.y = 0;
	handle->mouse.z = 0;
	handle->mouse.press_l = 0;
	handle->mouse.press_r = 0;

	*(uint16_t *)&handle->keyboard = 0;
	// 关闭接收,等待被看门狗咬死后重新开启
	SEML_UART_Abort(&handle->uart);
}

/**
 * @brief DR16失联复位回调函数
 * @param config 接收句柄
 */
void DR16_Rx_Timeout_Reset_Callack(void *config)
{
	RC_Ctrl_t *handle = config;
	handle->status = SEML_TIMEOUT;
	DR16_Reset(handle);
	SEML_UART_Receive(&handle->uart, handle->buffer, 18);
}

/**
 * @brief 获取DR16键盘数据
 * @param handle 遥控器句柄
 * @param key 遥控器按键枚举
 */
uint8_t DR16_Get_Key_Status(RC_Ctrl_t *handle,RC_Key_t key)
{
	return ((*(uint16_t*)&handle->keyboard & 0x0001 << key )!= 0);
}

/**
 * @brief 获取DR16鼠标左键数据
 * @param handle 遥控器句柄
 */
uint8_t DR16_Get_Mouse_Left_Status(RC_Ctrl_t *handle)
{
	return handle->mouse.press_l;
}

/**
 * @brief 获取DR16鼠标右键数据
 * @param handle 遥控器句柄
 */
uint8_t DR16_Get_Mouse_Right_Status(RC_Ctrl_t *handle)
{
	return handle->mouse.press_r;
}

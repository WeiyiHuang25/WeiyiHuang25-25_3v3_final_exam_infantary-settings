#include "SuperCap.h"
/**
 * @brief 超级电容句柄初始化
 * @param[out] handle 超级电容句柄
 * @param[in] can_handle can句柄
 */
void SuperCap_Init(SuperCap_t *handle, Can_Handle_t *can_handle)
{
	handle->feedbuck_value.Vchassis = 0;
	handle->feedbuck_value.Vcap = 0;
	handle->feedbuck_value.Pcharge = 0;
	handle->feedbuck_value.Pchassis = 0;
	handle->config_value.power_charge_limit = 150;
	handle->config_value.power_limit = 50;
	handle->config_value.power_compensation_limit = 255;
	handle->config_value.cap_enable = 0;
	handle->status.value = 0x00;
	// 内存初始化
	handle->can_handle = can_handle;
	SEML_CAN_Rxmessage_Register(can_handle, SUPERCAP_RX_ID, (message_callback_fun_t)SuperCap_Rx_Callback, handle);
	SuperCap_Sand_Config(handle);
}

/**
 * @brief 超级电容接收回调函数
 * @note 解算超级电容接收值
 * @param[out] handle 超级电容回调函数
 * @param[in] pack 接收数据包
*/
void SuperCap_Rx_Callback(SuperCap_t *handle, message_Pack_t const *const pack)
{
	uint8_t *buffer;
	uint16_t longth;
	longth = Get_message_Pack_Data(pack, (void**)&buffer);
	if(longth != 8)
		return;
	handle->feedbuck_value.Vcap = ((uint16_t)buffer[0] << 8 | (uint16_t)buffer[1]) * 0.01f;
	handle->feedbuck_value.Pcharge = ((int16_t)buffer[2] << 8 | (int16_t)buffer[3]) * 0.01f;
	handle->feedbuck_value.Pchassis = ((int16_t)buffer[4] << 8 | buffer[5]) * 0.01f;
	handle->feedbuck_value.Vchassis = buffer[7] * 0.1f;
	handle->status.value = buffer[6];
	handle->rx_timestamp = SEML_GetTick();
}

/**
 * @brief 向超级电容发送配置
 * @note 定期调用(最佳为10ms发送一次),以便及时更新超级电容配置信息
 * @param[in] handle 超级电容句柄
 */
void SuperCap_Sand_Config(SuperCap_t *handle)
{
	assert_param(handle != NULL);
	uint8_t txbuffer[8];

	txbuffer[0] = handle->config_value.power_limit;
	txbuffer[1] = 0;
	txbuffer[2] = handle->config_value.power_compensation_limit;
	txbuffer[3] = 0;
	txbuffer[4] = handle->config_value.power_charge_limit;
	txbuffer[5] = 0;
	txbuffer[6] = handle->config_value.cap_enable;
	
	SEML_CAN_Send(handle->can_handle, SUPERCAP_CONFIG_ID, txbuffer, 8, CAN_RTR_DATA);
}

/**
 * @brief 向超级电容发送裁判系统缓冲数据
 * @param[in] handle 超级电容句柄
 * @param[in] buffer_power 裁判系统发来的能量缓冲
 */
void SuperCup_Sand_Data(SuperCap_t *handle, float buffer_power)
{
	uint8_t txbuffer[8];
	uint16_t temp = buffer_power * 100;
	txbuffer[0] = temp >> 8;
	txbuffer[1] = temp;
	txbuffer[2] = 0;
	txbuffer[3] = 0;
	txbuffer[4] = 0;
	txbuffer[5] = 0;
	txbuffer[6] = 0;
	txbuffer[7] = 0;

	SEML_CAN_Send(handle->can_handle, SUPERCAP_DATA_ID, txbuffer, 8, CAN_RTR_DATA);
}


#ifndef __AHT20_H
#define __AHT20_H
#include "SEML_common.h"
#include "i2c_if.h"

typedef struct
{
	I2C_Handle_t i2c_handle;
	uint8_t alive;	// 0-器件不存在; 1-器件存在
	SEML_StatusTypeDef flag;		// 读取/计算错误标志位。0-读取/计算数据正常; 1-读取/计算设备失败
	uint32_t HT[2]; // 湿度、温度 原始传感器的值，20Bit

	float RH;		// 湿度，转换单位后的实际值，标准单位%
	float Temp; // 温度，转换单位后的实际值，标准单位°C
} AHT20_t;

#define AHT20_TIMEOUT 1
#define ATH20_SLAVE_ADDRESS 0x38 /* I2C从机地址 */

//****************************************
// 定义 AHT20 内部地址
//****************************************
#define AHT20_STATUS_REG 0x00			 // 状态字 寄存器地址
#define AHT20_INIT_REG 0xBE				 // 初始化 寄存器地址
#define AHT20_SoftReset 0xBA			 // 软复位 单指令
#define AHT20_TrigMeasure_REG 0xAC // 触发测量 寄存器地址

/**
 * @brief  AHT20 设备初始化
 * @param aht20 aht20句柄
 * @retval uint8_t：0 - 初始化AHT20设备成功; 1 - 初始化AHT20失败，可能设备不存在或器件已损坏
 */
SEML_StatusTypeDef AHT20_Init(AHT20_t *aht20);

/**
 * @brief AHT20读取温湿度数据
 * @param aht20 aht20句柄
 * @return uint8_t
 */
SEML_StatusTypeDef AHT20_Read(AHT20_t *aht20);

#endif
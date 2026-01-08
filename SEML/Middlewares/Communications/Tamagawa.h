#ifndef _TAMAGAWA_H_
#define _TAMAGAWA_H_
#include "SEML_common.h"
#include "uart_if.h"
/**
 * @brief 多摩川编码器指令
*/
typedef enum
{
	TAMAGAWA_DATA_ID0 = 0x02, /**< id0 绝对位置 */
	TAMAGAWA_DATA_ID1 = 0x8A, /**< id1 圈数 */
	TAMAGAWA_DATA_ID2 = 0x92, /**< id2 ENID */
	TAMAGAWA_DATA_ID3 = 0x12, /**< id3 绝对位置 圈数 错误码 ENID */
	TAMAGAWA_DATA_ID6 = 0x32, /**< id6 写入到EEPROM */
	TAMAGAWA_DATA_IDD = 0xEA, /**< idD 从EEPROM读取*/
	TAMAGAWA_DATA_ID7 = 0xBA, /**< id7 绝对位置 */
	TAMAGAWA_DATA_ID8 = 0xC2, /**< id8 绝对位置 */
	TAMAGAWA_DATA_IDC = 0x62, /**< idC 绝对位置 */
} TAMAGAWA_CF_t;

typedef enum
{
	TAMAGAWA_Get_Position = TAMAGAWA_DATA_ID0,
} TAMAGAWA_Cmd_t;

/**
 * @brief 多摩川编码器错误码
*/
typedef enum
{
	TAMAGAWA_ALMC_NO_ERROR = 0x00,						 /**< 无错误 */
	TAMAGAWA_ALMC_OVERSPEED = 0x01,						 /**< 超速 */
	TAMAGAWA_ALMC_FULL_ABSOLUTE_STATUS = 0x02, /**< ??? */
	TAMAGAWA_ALMC_COUNT_ERROR = 0x04,					 /**< 计数错误 */
	TAMAGAWA_ALMC_COUNT_OVERFLOW = 0x08,			 /**< 计数移除 */
	TAMAGAWA_ALMC_OVERHEAT = 0x10,						 /**< 过热 */
	TAMAGAWA_ALMC_MULTITURN_ERROR = 0x20,			 /**< ??? */
	TAMAGAWA_ALMC_BATTERY_ERROR = 0x40,				 /**< ??? */
	TAMAGAWA_ALMC_BATTERY_ALARM = 0x80,				 /**< ??? */
} TAMAGAWA_ALMC_t;

/**
 * @brief 多摩川编码器句柄
*/
typedef struct
{
	uint32_t position;					 /**< 编码器位置 */
	uint32_t cycles;						 /**< 周期 */
	TAMAGAWA_CF_t control_field; /**< 指令码 */
	UART_Handle_t uart_handle;	 /**< 串口句柄 */
	uint8_t rx_buffer[11];			 /**< 接收缓存 */
	uint8_t ALMC;								 /**< 错误码 */
	uint8_t ENID;								 /**< ??? */
} Tamagawa_Encode_t;

/**
 * @brief 多摩川编码器初始化
 * @param handle 多摩川编码器句柄
 * @param huart 串口接口句柄
 * @param tx_en_part 458发送使能端口
 * @param tx_en_pin 458发送使能引脚
 */
void Tamagawa_Init(Tamagawa_Encode_t *handle, UART_IF_t *huart, GPIO_IF_t *tx_en_part, uint16_t tx_en_pin);

/**
 * @brief 向多摩川编码器发送指令
 * @param handle 多摩川编码器句柄
 * @param cmd 指令
 */
void Tamagawa_Send_Cmd(Tamagawa_Encode_t *handle, TAMAGAWA_Cmd_t cmd);

/**
 * @brief 多摩川编码器接收回调函数
 * @param uart 串口句柄
 */
void Tamagawa_callback(UART_Handle_t *uart);

#endif
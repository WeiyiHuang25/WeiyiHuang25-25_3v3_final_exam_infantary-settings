/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : uart_if.h
 * @author : SY7_yellow
 * @brief  : uart接口文件
 * @date   : 2023-10-31
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-10-31     <td>1.0         <td>SY7_yellow  <td>实现了基础功能
 * <tr><td>2023-11-01     <td>1.1         <td>SY7_yellow  <td>统一了接口类型
 * <tr><td>2023-11-12     <td>1.2         <td>SY7_yellow  <td>添加了通讯终止函数
 * </table>
 *@details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 *
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 *
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef __UART_IF_H_
#define __UART_IF_H_
#include "SEML_common.h"
#include "gpio_if.h"
#ifdef USE_HAL_DRIVER
typedef UART_HandleTypeDef UART_IF_t;
#else
#error Error: uart interface is not configured(uart_if.h)
#endif
/**
 * @brief 串口传输模式
 */
typedef enum
{
	UART_Block_Mode,		 /**< 阻塞 */
	UART_DMA_Mode,			 /**< DMA */
	UART_IT_Mode,				 /**< 中断 */
	UART_DMA_Cycle_Mode, /**< DMA循环 */
	UART_IT_Cycle_Mode	 /**< 中断循环 */
} UART_Transmission_Mode_t;
/**
 * @brief 串口传输状态
 */
typedef enum
{
	UART_Stop = 0,
	UART_Run
}UART_Transmission_Status_t;

/**
 * @brief 串口句柄
 */
typedef struct
{
	UART_IF_t *huart;					/**< 串口接口句柄 */
	GPIO_Handle_t tx_en_gpio; /**< 发送使能io */
	enum
	{
		UART_Type_TTL = 0,									/**< TTL模式 */
		UART_Type_RS485,										/**< 485模式 */
	} type : 4;														/**< 串口模式 */
	UART_Transmission_Mode_t tx_mode : 3; /**< 发送模式 */
	UART_Transmission_Status_t tx_status : 1;/**< 发送状态 */
	UART_Transmission_Mode_t rx_mode : 3; /**< 接收模式 */
	UART_Transmission_Status_t rx_status : 1;/**< 接收状态 */
	uint8_t *rx_buffer;										/**< 接收缓存 */
	uint16_t rx_buffer_longth;						/**< 接收缓存长度 */
	uint32_t rx_start_tick;
	uint16_t rx_count;										/**< 已经接收数据长度 */
	void (*rx_callback_fun)(void *uart);	/**< 接收回调函数 */
	void *rx_callback_config;							/**< 接收回调函数配置 */
	uint32_t timeout;											/**< 超时时间(阻塞模式使用) */
	void *next;
} UART_Handle_t;

typedef void (*Rx_Callback_t)(UART_Handle_t *uart);

/**
 * @brief 串口初始化
 * @param uart 串口句柄
 * @param tx_mode 发送模式
 * @param rx_mode 接收模式
 * @param timeout 超时时间
 * @param huart 串口接口句柄
 */
void SEML_UART_Init(UART_Handle_t *uart, UART_Transmission_Mode_t tx_mode, UART_Transmission_Mode_t rx_mode, uint32_t timeout, UART_IF_t *huart);

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
void SEML_RS_485_Init(UART_Handle_t *uart, UART_Transmission_Mode_t tx_mode, UART_Transmission_Mode_t rx_mode, uint32_t timeout, UART_IF_t *huart, GPIO_IF_t *tx_en_part, uint16_t tx_en_pin);

/**
 * @brief 串口发送函数
 * @param uart 串口句柄
 * @param data 数据指针
 * @param longth 数据长度
 * @return 函数运行状态
 */
SEML_StatusTypeDef SEML_UART_Transmit(UART_Handle_t *uart, uint8_t *data, uint8_t longth);

/**
 * @brief 串口接收函数
 * @param uart 串口句柄
 * @param data 数据指针
 * @param longth 数据长度
 * @return 函数运行状态
 */
SEML_StatusTypeDef SEML_UART_Receive(UART_Handle_t *uart, uint8_t *data, uint16_t longth);

/**
 * @brief 注册串口接收回调函数
 * @param uart 串口句柄
 * @param rx_callback_fun 接收回调函数
 * @param rx_callback_config 接收回调函数配置
 */
void SEML_UART_Register_Callback(UART_Handle_t *uart, Rx_Callback_t rx_callback_fun, void *rx_callback_config);

/**
 * @brief 串口通讯终止函数
 * @param uart 串口句柄
 */
void SEML_UART_Abort(UART_Handle_t *uart);

/**
 * @brief 串口接收完成中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UART_Rx_Complete_Callback(UART_IF_t *huart);

/**
 * @brief 串口发送完成中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UART_Tx_Complete_Callback(UART_IF_t *huart);

/**
 * @brief 串口接收空闲中断回调函数
 * @param config 串口接口句柄
 */
void SEML_UARTEx_RxEventCallback(UART_IF_t *huart);

/**
 * @brief 获取回调函数配置
 * @param handle 串口句柄
 */
#define SEML_UART_Get_Rx_Callback_Config(handle) ((handle)->rx_callback_config)

/**
 * @brief 获取接收到的数据长度
 * @attention 仅在中断中使用
 * @param handle 串口句柄
 */
#define SEML_UART_Get_Rx_Count(handle) ((handle)->rx_count)

#endif

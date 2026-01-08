/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       radio.h
 * \brief      Generic radio driver ( radio abstraction )
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Gregory Cristian on Apr 25 2013
 */
#ifndef __RADIO_H__
#define __RADIO_H__
/**
 * @addtogroup 射频模块操作函数
 * @{
 */
/**
 * @brief SX1272和SX1276通用参数定义
 */
#define LORA                                        1         // [0: OFF, 1: ON]

/**
 * @brief RF处理功能返回码
 */
typedef enum
{
    RF_IDLE,                        /**< RF空闲 */
    RF_BUSY,                        /**< RF忙 */
    RF_RX_DONE,                     /**< RF接收完成 */
    RF_RX_TIMEOUT,                  /**< RF接收超时 */
    RF_TX_DONE,                     /**< RF发送完成 */
    RF_TX_TIMEOUT,                  /**< RF发送超时 */
    RF_LEN_ERROR,                   /**< RF长度错误 */
    RF_CHANNEL_EMPTY,               /**< RF通道空闲 */
    RF_CHANNEL_ACTIVITY_DETECTED,   /**< RF检测到通道占用 */
}tRFProcessReturnCodes;

/**
 * @brief 定义了不同函数指针的无线电驱动程序结构
 */
typedef struct sRadioDriver
{
    void ( *Init )( void );                                     /**< 初始化 */
    void ( *Reset )( void );                                    /**< 重置 */
    void ( *StartRx )( void );                                  /**< 开始接收 */
    void ( *GetRxPacket )( void *buffer, uint16_t *size );      /**< 获取接收数据 */
    void ( *SetTxPacket )( const void *buffer, uint16_t size ); /**< 设置发送数据 */
    uint32_t ( *Process )( void );                              /**< 获取当前接收状态 */
}tRadioDriver;
#if defined( USE_SX1276_RADIO )
/// @brief 射频模块初始化
#define Radio_Init() SX1276Init()
/// @brief 射频模块重置
#define Radio_Reset() SX1276Reset()
/// @brief 射频模块开始接收
#define Radio_StartRx() SX1276StartRx()
/// @brief 获取射频接收数据
/// @param[out] buffer 接收缓存数组
/// @param[out] size 接收数据大小
#define Radio_GetRxPacket(buffer,size) SX1276GetRxPacket(buffer,size)
/// @brief 设置射频发送数据
/// @param[in] buffer 发送缓存数组
/// @param[in] size 发送数据大小
#define Radio_SetTxPacket(buffer,size) SX1276SetTxPacket(buffer,size)
/// @brief 获取射频工作模式处理Rx和Tx状态
#define Radio_Process( void ) SX1276Process()
#else
    #error "Missing define function:Radio_xxxx"
#endif    
/**
 * @brief 用特定的无线电函数初始化RadioDriver结构体
 * @retval radioDriver指针指向无线电驱动变量
 */
tRadioDriver* RadioDriverInit( void );
/// @} 射频模块操作函数
#endif // __RADIO_H__

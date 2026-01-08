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
 * \file       sx1276.h
 * \brief      SX1276 RF chip driver
 *
 * \version    2.0.B2 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1276_H__
#define __SX1276_H__
/**
 * @addtogroup SX1276操作函数
 * @{
 */
/*!
 * \brief SX1276 registers array
 */
extern uint8_t SX1276Regs[0x70];

/*!
 * \brief Enables LoRa modem or FSK modem
 *
 * \param [IN] opMode New operating mode
 */
/**
 * @brief 开启SX1275的LoRa调制解调器
 * @param enable 使能状态
 */
void SX1276SetLoRaOn( bool enable );

/*!
 * \brief Gets the LoRa modem state
 *
 * \retval LoraOn Current LoRa modem mode
 */
/**
 * @brief 获取SX1276的LoRa使能情况
 * @return true 使能
 * @return false 失能
 */
bool SX1276GetLoRaOn( void );

/*!
 * \brief Initializes the SX1276
 */
/**
 * @brief 初始化SX1276
 */
void SX1276Init( void );

/*!
 * \brief Resets the SX1276
 */
/**
 * @brief 重置SX1276
 */
void SX1276Reset( void );

/*!
 * \brief Sets the SX1276 operating mode
 *
 * \param [IN] opMode New operating mode
 */
/**
 * @brief 设置SX1276的opMode寄存器
 * @param opMode opMode模式
 */
void SX1276SetOpMode( uint8_t opMode );

/*!
 * \brief Gets the SX1276 operating mode
 *
 * \retval opMode Current operating mode
 */
/**
 * @brief 获取SX1276的opMode寄存器内的值
 * @return opMode寄存器值
 */
uint8_t SX1276GetOpMode( void );

/*!
 * \brief Reads the current Rx gain setting
 *
 * \retval rxGain Current gain setting
 */
/**
 * @brief 获取SX1276的接收增益
 * @return LnaGain设置值
 */
uint8_t SX1276ReadRxGain( void );

/*!
 * \brief Trigs and reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
/**
 * @brief 获取SX1276的接收强度指示(RSSI)
 * @return RSSI绝对值(dBm)
 */
double SX1276ReadRssi( void );

/*!
 * \brief Gets the Rx gain value measured while receiving the packet
 *
 * \retval rxGainValue Current Rx gain value
 */
/**
 * @brief 获取SX1276接收数据包时测量的Rx增益值
 * @return 当前Rx增益值
 */
uint8_t SX1276GetPacketRxGain( void );

/*!
 * \brief Gets the SNR value measured while receiving the packet
 *
 * \retval snrValue Current SNR value in [dB]
 */
/**
 * @brief 获取SX1276接收数据包时测量的SNR值
 * @return 当前信噪比值，单位为[dB]
 */
int8_t SX1276GetPacketSnr( void );

/*!
 * \brief Gets the RSSI value measured while receiving the packet
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
/**
 * @brief 获取SX1276接收数据包时测量的RSSI值
 * @return 当前RSSI值[dBm] 
 */
double SX1276GetPacketRssi( void );

/*!
 * \brief Gets the AFC value measured while receiving the packet
 *
 * \retval afcValue Current AFC value in [Hz]
 */
/**
 * @brief 获取接收数据包时测量的AFC值
 * @return 当前AFC值[Hz]
 */
uint32_t SX1276GetPacketAfc( void );

/*!
 * \brief Sets the radio in Rx mode. Waiting for a packet
 */
/**
 * @brief 将SX1276设置为Rx模式,等待数据包
 */
void SX1276StartRx( void );

/*!
 * \brief Gets a copy of the current received buffer
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
/**
 * @brief 获取接收数据
 * @param[out] buffer 接收数据缓存数组
 * @param[out] size 接收数据包大小
 */
void SX1276GetRxPacket( void *buffer, uint16_t *size );

/*!
 * \brief Sets a copy of the buffer to be transmitted and starts the
 *        transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
/**
 * @brief 设置发送数据
 * @param[in] buffer 发送数据缓存
 * @param[in] size 发送数据大小
 */
void SX1276SetTxPacket( const void *buffer, uint16_t size );

/*!
 * \brief Gets the current RFState
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
/**
 * @brief 获取SX1276当前的射频状态
 * 
 * @return 当前射频状态
 * @retval RF_IDLE RF空闲
 * @retval RF_BUSY RF忙
 * @retval RF_RX_DONE RF接收完成
 * @retval RF_RX_TIMEOUT RF接收超时
 * @retval RF_TX_DONE RF发送完成
 * @retval RF_TX_TIMEOUT RF发送超时
 */
uint8_t SX1276GetRFState( void );

/*!
 * \brief Sets the new state of the RF state machine
 *
 * \param [IN]: state New RF state machine state
 */
/**
 * @brief 获取SX1276当前的射频状态
 * @param[in] state 新状态
 */
void SX1276SetRFState( uint8_t state );

/*!
 * \brief Process the Rx and Tx state machines depending on the
 *       SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
/**
 * @brief 根据SX1276的工作模式处理Rx和Tx状态
 * @return 当前射频状态
 * @retval RF_IDLE RF空闲
 * @retval RF_BUSY RF忙
 * @retval RF_RX_DONE RF接收完成
 * @retval RF_RX_TIMEOUT RF接收超时
 * @retval RF_TX_DONE RF发送完成
 * @retval RF_TX_TIMEOUT RF发送超时
 */
uint32_t SX1276Process( void );
/// @}SX1276操作函数
#endif //__SX1276_H__

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
 * \file       sx1276.c
 * \brief      SX1276 RF chip driver
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "main.h"
#include "radio.h"

#if defined( USE_SX1276_RADIO )

#include "sx1276.h"

#include "sx1276-Hal.h"
#include "sx1276-Fsk.h"
#include "sx1276-LoRa.h"

/*!
 * SX1276 registers variable
 */
uint8_t SX1276Regs[0x70];

static bool LoRaOn = false;
static bool LoRaOnState = false;

/**
 * @brief 初始化SX1276
 */
void SX1276Init( void )
{
		uint8_t TempReg;
	
    // 初始化FSK和LoRa寄存器结构
    SX1276 = ( tSX1276* )SX1276Regs;
    SX1276LR = ( tSX1276LR* )SX1276Regs;

    SX1276InitIo( );
    
    SX1276Reset( );

		// 测试SX1276是否存在
		SX1276Read(0x06,&TempReg);
		while(TempReg != 0x6C)
		{
//			printf("Hard SPI Err!\r\n");
			HAL_Delay(100);
		}
		
    // 射频复位后，默认调制解调器为FSK

#if ( LORA == 0 ) 

    LoRaOn = false;
    SX1276SetLoRaOn( LoRaOn );
    // 初始化 FSK 调制解调器
    SX1276FskInit( );

#else

    LoRaOn = true;
    SX1276SetLoRaOn( LoRaOn );
    // 初始化 LoRa 调制解调器
    SX1276LoRaInit( );
    
#endif

}

/**
 * @brief 重置SX1276
 */
void SX1276Reset( void )
{
    SX1276SetReset( RADIO_RESET_ON );
    
    // Wait 1ms
    uint32_t startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1276SetReset( RADIO_RESET_OFF );
    
    // Wait 6ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
}

/**
 * @brief 开启SX1275的LoRa调制解调器
 * @param enable 使能状态
 */
void SX1276SetLoRaOn( bool enable )
{
    if( LoRaOnState == enable )
    {
        return;
    }
    LoRaOnState = enable;
    LoRaOn = enable;

    if( LoRaOn == true )
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
        SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
        SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
        
        SX1276ReadBuffer( REG_LR_OPMODE, SX1276Regs + 1, 0x70 - 1 );
    }
    else
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF;
        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        
        SX1276ReadBuffer( REG_OPMODE, SX1276Regs + 1, 0x70 - 1 );
    }
}

/**
 * @brief 获取SX1276的LoRa使能情况
 * @return true 使能
 * @return false 失能
 */
bool SX1276GetLoRaOn( void )
{
    return LoRaOn;
}
/**
 * @brief 设置SX1276的opMode寄存器
 * @param opMode opMode模式
 */
void SX1276SetOpMode( uint8_t opMode )
{
    if( LoRaOn == false )
    {
        SX1276FskSetOpMode( opMode );
    }
    else
    {
        SX1276LoRaSetOpMode( opMode );
    }
}

/**
 * @brief 获取SX1276的opMode寄存器内的值
 * @return opMode寄存器值
 */
uint8_t SX1276GetOpMode( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetOpMode( );
    }
    else
    {
        return SX1276LoRaGetOpMode( );
    }
}
/**
 * @brief 获取SX1276的接收强度指示(RSSI)
 * @return RSSI绝对值(dBm)
 */
double SX1276ReadRssi( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskReadRssi( );
    }
    else
    {
        return SX1276LoRaReadRssi( );
    }
}

/**
 * @brief 获取SX1276的接收增益
 * @return LnaGain设置值
 */
uint8_t SX1276ReadRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskReadRxGain( );
    }
    else
    {
        return SX1276LoRaReadRxGain( );
    }
}

/**
 * @brief 获取SX1276接收数据包时测量的Rx增益值
 * @return 当前Rx增益值
 */
uint8_t SX1276GetPacketRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetPacketRxGain(  );
    }
    else
    {
        return SX1276LoRaGetPacketRxGain(  );
    }
}

/**
 * @brief 获取SX1276接收数据包时测量的SNR值
 * @return 当前信噪比值，单位为[dB]
 */
int8_t SX1276GetPacketSnr( void )
{
    if( LoRaOn == false )
    {
         while( 1 )
         {
             // Useless in FSK mode
             // Block program here
         }
    }
    else
    {
        return SX1276LoRaGetPacketSnr(  );
    }
}

/**
 * @brief 获取SX1276接收数据包时测量的RSSI值
 * @return 当前RSSI值[dBm] 
 */
double SX1276GetPacketRssi( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetPacketRssi(  );
    }
    else
    {
        return SX1276LoRaGetPacketRssi( );
    }
}

/**
 * @brief 获取接收数据包时测量的AFC值
 * @return 当前AFC值[Hz]
 */
uint32_t SX1276GetPacketAfc( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetPacketAfc(  );
    }
    else
    {
         while( 1 )
         {
             // Useless in LoRa mode
             // Block program here
         }
    }
}

/**
 * @brief 将SX1276设置为Rx模式,等待数据包
 */
void SX1276StartRx( void )
{
    if( LoRaOn == false )
    {
        SX1276FskSetRFState( RF_STATE_RX_INIT );
    }
    else
    {
        SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
    }
}

/**
 * @brief 获取接收数据
 * @param[out] buffer 接收数据缓存数组
 * @param[out] size 接收数据包大小
 */
void SX1276GetRxPacket( void *buffer, uint16_t *size )
{
    if( LoRaOn == false )
    {
        SX1276FskGetRxPacket( buffer, size );
    }
    else
    {
        SX1276LoRaGetRxPacket( buffer, size );
    }
}

/**
 * @brief 设置发送数据
 * @param[in] buffer 发送数据缓存
 * @param[in] size 发送数据大小
 */
void SX1276SetTxPacket( const void *buffer, uint16_t size )
{
    if( LoRaOn == false )
    {
        SX1276FskSetTxPacket( buffer, size );
    }
    else
    {
        SX1276LoRaSetTxPacket( buffer, size );
    }
}

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
uint8_t SX1276GetRFState( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetRFState( );
    }
    else
    {
        return SX1276LoRaGetRFState( );
    }
}

/**
 * @brief 获取SX1276当前的射频状态
 * @param[in] state 新状态
 */
void SX1276SetRFState( uint8_t state )
{
    if( LoRaOn == false )
    {
        SX1276FskSetRFState( state );
    }
    else
    {
        SX1276LoRaSetRFState( state );
    }
}

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
uint32_t SX1276Process( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskProcess( );
    }
    else
    {
        return SX1276LoRaProcess( );
    }
}

#endif // USE_SX1276_RADIO

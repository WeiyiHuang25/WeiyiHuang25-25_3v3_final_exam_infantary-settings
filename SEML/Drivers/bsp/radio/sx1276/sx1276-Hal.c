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
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <stdint.h>
#include <stdbool.h> 
#include "main.h"

#if defined( USE_SX1276_RADIO )

//#include "ioe.h"
#include "spi.h"
#include "sx1276-Hal.h"

#ifdef USE_SEML_LIB
/**
 * SX1276 SPI句柄定义
 */
SPI_Handle_t SX1276_SPI;

/*!
 * SX1276 RESET I/O definitions
 */
GPIO_Handle_t SX1276_RESET = 
{
#ifdef SX_REST_GPIO_Port
    .GPIO_Part = SX_REST_GPIO_Port,
    .GPIO_Pin  = SX_REST_Pin
#else
#error The REST pin of SX1276 is not initialized
#endif
};

/*!
 * SX1276 SPI NSS I/O definitions
 */
GPIO_Handle_t SX1276_NSS = 
{
#ifdef SX_NSS_GPIO_Port
    .GPIO_Part = SX_NSS_GPIO_Port,
    .GPIO_Pin  = SX_NSS_Pin
#else
#error The NSS pin of SX1276 is not initialized
#endif
};
/*!
 * SX1276 DIO pins  I/O definitions
 */
GPIO_Handle_t SX1276_DIO0 = 
{
#ifdef SX_DIO0_GPIO_Port
    .GPIO_Part = SX_DIO0_GPIO_Port,
    .GPIO_Pin  = SX_DIO0_Pin
#else
#error The DIO0 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};
GPIO_Handle_t SX1276_DIO1 = 
{
#ifdef SX_DIO1_GPIO_Port
    .GPIO_Part = SX_DIO1_GPIO_Port,
    .GPIO_Pin  = SX_DIO1_Pin
#else
#warning The DIO1 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};
GPIO_Handle_t SX1276_DIO2 = 
{
#ifdef SX_DIO2_GPIO_Port
    .GPIO_Part = SX_DIO2_GPIO_Port,
    .GPIO_Pin  = SX_DIO2_Pin
#else
#warning The DIO2 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};
GPIO_Handle_t SX1276_DIO3 = 
{
#ifdef SX_DIO3_GPIO_Port
    .GPIO_Part = SX_DIO3_GPIO_Port,
    .GPIO_Pin  = SX_DIO3_Pin
#else
#warning The DIO3 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};
GPIO_Handle_t SX1276_DIO4 = 
{
#ifdef SX_DIO4_GPIO_Port
    .GPIO_Part = SX_DIO4_GPIO_Port,
    .GPIO_Pin  = SX_DIO4_Pin
#else
#warning The DIO4 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};
GPIO_Handle_t SX1276_DIO5 = 
{
#ifdef SX_DIO5_GPIO_Port
    .GPIO_Part = SX_DIO5_GPIO_Port,
    .GPIO_Pin  = SX_DIO5_Pin
#else
#warning The DIO5 pin of SX1276 is not initialized
    .GPIO_Part = NULL,
    .GPIO_Pin  = 0
#endif
};

#else // USE_SEML_LIB

/*!
 * SX1276 RESET I/O definitions
 */
#define RESET_IOPORT                                SX_REST_GPIO_Port
#define RESET_PIN                                   SX_REST_Pin

/*!
 * SX1276 SPI NSS I/O definitions
 */
#define NSS_IOPORT                                  SX_NSS_GPIO_Port
#define NSS_PIN                                     SX_NSS_Pin

/*!
 * SX1276 DIO pins  I/O definitions
 */
#define DIO0_IOPORT                                 SX_DIO0_GPIO_Port
#define DIO0_PIN                                    SX_DIO0_Pin

#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_PIN_1

#define DIO2_IOPORT                                 GPIOB
#define DIO2_PIN                                    GPIO_PIN_2

#define DIO3_IOPORT                                 GPIOB
#define DIO3_PIN                                    GPIO_PIN_3

#define DIO4_IOPORT                                 GPIOB
#define DIO4_PIN                                    GPIO_PIN_4

#define DIO5_IOPORT                                 GPIOB
#define DIO5_PIN                                    GPIO_PIN_5

uint8_t SpiInOut( uint8_t outData )
{
	uint8_t pData = 0;

  if(HAL_SPI_TransmitReceive(&hspi1,&outData,&pData,1,0xffff) != HAL_OK)
	{
		return ERROR;
	}    
  else
	{
		return pData;
	}	
}

#endif // USE_SEML_LIB

void SX1276InitIo( void )
{
#ifdef USE_SEML_LIB
    SEML_GPIO_Pin_Set(&SX1276_NSS);
#else
	HAL_GPIO_WritePin(NSS_IOPORT, NSS_PIN, GPIO_PIN_SET);
#endif
}

void SX1276SetReset( uint8_t state )
{
    if( state == RADIO_RESET_ON )
    {
#ifdef USE_SEML_LIB
        SEML_GPIO_Pin_Reset(&SX1276_RESET);
#else
		HAL_GPIO_WritePin(RESET_IOPORT, RESET_PIN, GPIO_PIN_RESET);
#endif
    }
    else
    {
#ifdef USE_SEML_LIB
        SEML_GPIO_Pin_Set(&SX1276_RESET);
#else		
		HAL_GPIO_WritePin(RESET_IOPORT, RESET_PIN, GPIO_PIN_SET);
#endif
    }
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    uint8_t temp =  addr | 0x80 ;
    //NSS = 0;
#ifdef USE_SEML_LIB
    SEML_GPIO_Pin_Reset(&SX1276_NSS);

    SEML_SPI_Transmit(&SX1276_SPI,&temp,1,10);
    SEML_SPI_Transmit(&SX1276_SPI,buffer,size,10);

    SEML_GPIO_Pin_Set(&SX1276_NSS);
#else
	HAL_GPIO_WritePin(NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET);
    SpiInOut( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiInOut( buffer[i] );
    }
    HAL_GPIO_WritePin(NSS_IOPORT, NSS_PIN, GPIO_PIN_SET);
#endif
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    uint8_t temp =  addr & 0x7F ;
#ifdef USE_SEML_LIB
    SEML_GPIO_Pin_Reset(&SX1276_NSS);

    SEML_SPI_Transmit(&SX1276_SPI,&temp,1,10);
    SEML_SPI_Receive(&SX1276_SPI,buffer,size,10);

    SEML_GPIO_Pin_Set(&SX1276_NSS);
#else
	HAL_GPIO_WritePin(NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET);
    
    SpiInOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( 0 );
    }

    HAL_GPIO_WritePin(NSS_IOPORT, NSS_PIN, GPIO_PIN_SET);
#endif

}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO0);
#else
	return HAL_GPIO_ReadPin(DIO0_IOPORT, DIO0_PIN);
#endif
}

inline uint8_t SX1276ReadDio1( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO1);
#else
	return HAL_GPIO_ReadPin(DIO1_IOPORT, DIO1_PIN);
#endif
}

inline uint8_t SX1276ReadDio2( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO2);
#else
	return HAL_GPIO_ReadPin(DIO2_IOPORT, DIO2_PIN);
#endif
}

inline uint8_t SX1276ReadDio3( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO3);
#else
	return HAL_GPIO_ReadPin(DIO3_IOPORT, DIO3_PIN);
#endif
}

inline uint8_t SX1276ReadDio4( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO4);
#else
	return HAL_GPIO_ReadPin(DIO4_IOPORT, DIO4_PIN);
#endif
}

inline uint8_t SX1276ReadDio5( void )
{
#ifdef USE_SEML_LIB
    return SEML_GPIO_Pin_Input(&SX1276_DIO5);
#else
	return HAL_GPIO_ReadPin(DIO5_IOPORT, DIO5_PIN);
#endif
}

// inline void SX1276WriteRxTx( uint8_t txEnable )
// {
//     if( txEnable != 0 )
//     {
// 			Set_RF_Switch_TX();
// //        IoePinOn( FEM_CTX_PIN );
// //        IoePinOff( FEM_CPS_PIN );
//     }
//     else
//     {
// 			Set_RF_Switch_RX();  
// //        IoePinOff( FEM_CTX_PIN );
// //        IoePinOn( FEM_CPS_PIN );
//     }
// }

#endif // USE_SX1276_RADIO

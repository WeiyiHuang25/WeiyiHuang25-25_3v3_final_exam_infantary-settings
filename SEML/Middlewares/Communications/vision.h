#ifndef _VISION_H_
#define _VISION_H_

#include "SEML_common.h"
#include "uart_if.h"
#include "timer.h"
#include <stdlib.h>
#include <stdbool.h>
#include "reference.h"
#include "Robo_gimbal.h"
#include "buzzer.h"

//斜坡函数步长
#define Vision_Step 0.0005f

//和视觉通信发送接收数据长度
#define  Vision_Send_BufferSize   13
#define  Vision_Get_BufferSize    10
/*----参数常量-----------------------------*/
//帧头帧尾
#define DATA_FARME_HEADER		0xAF
#define DATA_END_OF_FARME		0xFA
#define USE_SOFT_WITCHDOG       1
#define Vision_LOST_TIME ((uint32_t)100)

/* 数组编号 */
typedef enum 
{
	ID_FARME_HEADER = 0,
	ID_YAW_HIGH 	= 1,
	ID_YAW_LOW 		= 2,
	ID_PITCH_HIGH 	= 3,
	ID_PITCH_LOW 	= 4,
	ID_ROLL_HIGH 	= 5,
	ID_ROLL_LOW 	= 6,
	ID_SHOOT_SPEED_HIGH 	= 7,
	ID_SHOOT_SPEED_LOW 		= 8,
	ID_COLOUR = 9,
	ID_MODE   = 10,
	ID_CHECK        = Vision_Send_BufferSize -2,
	ID_END_OF_FARME = Vision_Send_BufferSize -1,	
}Vision_Send_Array_ID_t;

/*
*  云台接受视觉数据结构体                                 
*/
typedef struct
{	
    // 帧头  帧尾                                 
	uint8_t Frame_Header; 
	uint8_t End_Of_Frame;          
	
	int16_t Receive_Yaw;
	int16_t Receive_Pitch;
	
	uint8_t Shoot_Mode;
	uint16_t Shoot_Distance;
	
	//异或校验和
	uint8_t CheckSum;

	uint8_t rx_buffer[Vision_Get_BufferSize]; /**< 接收缓冲句柄 */

	struct
	{
		SEML_StatusTypeDef data_validity : 1; /**< 数据有效性 */
	} status;

#ifdef USE_SOFT_WITCHDOG
	Soft_WitchDog_t witchdog; /**< 软件看门狗*/
#endif

}GIM_By_Vision_t;

/*
*  向视觉发送数据结构体                                   
*/
typedef struct
{
 	uint8_t Frame_Header; 
	uint8_t End_Of_Frame; 

	int16_t Send_Yaw;
	int16_t Send_Pitch;
	int16_t Send_Roll;
	
	int16_t Shoot_Speed;
	
	bool COLOUR; //己方颜色(RED :1 BLUE : 0 )
	uint8_t MODE; //模式(自瞄 :0 ，反小陀螺：1，打符 : 2 )
	
	//异或校验和
	uint8_t CheckSum;

}GIM_To_Vision_t;

/*
* 视觉结构体
*/
typedef struct 
{
	SEML_StatusTypeDef uart_status; /**< 串口当前状态 */
	GIM_To_Vision_t Send_to_vision;
	GIM_By_Vision_t Receive_by_vision;
	UART_Handle_t uart;

	float Last_Time_Receive_Yaw; //上一时刻数据
	float Last_Time_Receive_Pitch;
	float Solved_Yaw; //斜坡函数后数据
	float Solved_Pitch;
	float Yaw_angle; // 弧度制数据
	float Pitch_angle;
	float temp_yaw; // 刚接收初始数据
	float temp_pitch;

#ifdef USE_SOFT_WITCHDOG
	Soft_WitchDog_t witchdog; /**< 软件看门狗*/
#endif
}Vision_t;



/**
	* @brief 视觉初始化
	* @param[out] handle 视觉句柄
 	* @param[in] huart 视觉挂载的串口句柄
*/
void Vision_Init(Vision_t *handle, UART_IF_t *huart);

/**
	* @brief 发送数据给视觉数据处理函数
	* @param[out] handle 裁判系统句柄
 	* @param[in] huart 视觉挂载的串口句柄
*/
void Send_To_Vision_t(Reference_t *reference_data, Vision_t *handle);

/**
	* @brief 视觉接收回调函数
	* @param[in] uart 串口句柄
*/
void Vision_Rx_Callback(UART_Handle_t *uart);

/**
	* @brief 视觉超时回调函数
	* @param[out] handle 视觉句柄
*/
void Vision_Timeout_Callback(Vision_t *handle);

/**
	* @brief 解析收到的视觉数据
	* @param[in,out] handle 视觉句柄
*/
SEML_StatusTypeDef Vision_Rx_Handle(Vision_t *handle);

void Vision_Control(void);
extern Vision_t vision;
/**
 * @brief 获取视觉pitch角度
 */
#define Get_Vision_Pitch_Angle(vision) ((vision)->Receive_by_vision.Receive_Pitch)

/**
 * @brief 获取视觉Yaw角度
 */
#define Get_Vision_Yaw_Angle(vision) ((vision)->Receive_by_vision.Receive_Yaw)

/**
 * @brief 获取当前目标距离
 */
#define Get_Vision_Distance_Data(vision) ((vision)->Receive_by_vision.Shoot_Distance)

/**
 * @brief 获取当前模式
 */
#define Get_Vision_Mode_Data(vision) ((vision)->Receive_by_vision.Shoot_Mode)

/**
 * @brief 获取数据有效性
 */
#define Get_Vision_status(vision) ((vision)->uart_status)


#endif

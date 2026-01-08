/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      usb outputs the error message.usb输出错误信息
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Nov-11-2019     RM              1. done
  *	 V2.0.0			Dec-27-2023			HansonZhang			2. added realtime PID tuning capability
  *  V3.0.0			Jan-4-2024			HansonZhang			3. PC communication and  auto aiming capability
  *	 V3.0.1			Jan-5-2024			HansonZhang			4. bug fixed: PC communication failed when auto aiming enabled
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#include "Robo_USB.h"
#include "gpio.h"
#include "Robo_Common.h"
#include "usart.h"

#include "stdint.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

//Prototypes of functions
static void usb_printf(const char *fmt,...);
static void send(ReceivePacket *packet);
static void receive(void);
extern uint32_t receive_handler(uint8_t* Buf, uint32_t Len);
static void init_packet(ReceivePacket *packet);

extern void set_send_packet(ReceivePacket *packet);
extern SendPacket get_received_packet(void);

static void demo_show_receive(void);
static void navigation_handler(void);

static void send_ext_packet(void);
//End of prototypes

static uint8_t usb_buf[256];
uint32_t usb_buf_pos=0;
uint32_t count=0;

static const char status[2][7] = {"OK", "ERROR!"};
//const error_t *error_list_usb_local;
static uint8_t usb_send[256];

/*
static uint8_t spi_send[256];
static uint8_t spi_empty[256];
static uint8_t spi_receive[256];
*/

// extern gimbal_control_t gimbal_control;
// extern ext_game_robot_state_t robot_state;
// extern chassis_move_t chassis_move;
// extern ext_power_heat_data_t power_heat_data_t;
// extern ext_game_robot_HP_t game_robot_HP_t;

ReceivePacket send_packet;
SendPacket received_packet;
SendNavi received_navi;
EXTReceivePacket ext_send_packet;

// Reference_t refere;
uint32_t data_length=sizeof(SendPacket);
navi navigation;

float demoA=0.0f;
float demoB=0.0f;
float demoC=0.0f;

void USB_Init()
{
    MX_USB_DEVICE_Init();
	//Reference_Init(&refere,&huart6);
	init_packet(&send_packet);
}

void USB_Task(void *conifg)
{
	receive();
	send(&send_packet);
	// send_ext_packet();
}

SendPacket get_received_packet(void)
{
	return received_packet;
}
void set_send_packet(ReceivePacket *packet)
{
	send_packet=*packet;
}

static void send(ReceivePacket *packet)
{
	memcpy(usb_send, packet, sizeof(ReceivePacket)-2);
  	packet->checksum = get_CRC16_check_sum(usb_send, sizeof(ReceivePacket)-2, 0xffff);
	memcpy(usb_send, packet, sizeof(ReceivePacket));
	CDC_Transmit_FS(usb_send, sizeof(ReceivePacket));
}

static void send_r(EXTReceivePacket *packet)
{
	memcpy(usb_send, packet, sizeof(EXTReceivePacket)-2);
  	packet->checksum = get_CRC16_check_sum(usb_send, sizeof(EXTReceivePacket)-2, 0xffff);
	memcpy(usb_send, packet, sizeof(EXTReceivePacket));
	CDC_Transmit_FS(usb_send, sizeof(EXTReceivePacket));
}

static void receive(void)
{
	uint32_t len = 1024;
	CDC_Receive_FS(usb_buf, &len); // Read data into the buffer
	if (usb_buf[0] == AUTO_AIMING_HEADER)
	{
		memcpy(&received_packet, usb_buf, sizeof(SendPacket));
	}

	else if (usb_buf[0] == NAVIGATION_HEADER)
	{
		memcpy(&received_navi, usb_buf, sizeof(SendNavi));
		navigation_handler();
	}
	else
	{
		navigation.vx = 0.0f;
		navigation.vy = 0.0f;
		navigation.vw = 0.0f;
	}
}

static void navigation_handler(void)
{
	
	float value1 = received_navi.linear_vx;
	float value2 = received_navi.linear_vy;
	float value3 = received_navi.angular_x;	

	Robo_Push_Message_Cmd("Forward",value1);
	Robo_Push_Message_Cmd("Left",value2);
	Robo_Push_Message_Cmd("Rotate",value3);	
}

static void init_packet(ReceivePacket *packet)
{
	packet->header=0x5A;
    packet->detect_color=0;
    packet->reset_tracker=0;
    packet->reserved=0;
    packet->roll=0.0f;
    packet->pitch=0.0f;
    packet->yaw=0.0f;
    packet->aim_x=0.0f;
    packet->aim_y=0.0f;
    packet->aim_z=0.0f;
}

static void init_ext_packet(EXTReceivePacket *packet)
{
	packet->header=EXT_HEADER;
	packet->robot_id=0;
}

static void send_ext_packet(void)
{
	ext_send_packet.robot_id = chassis.referee.game_robot_status.robot_id;;
	send_r(&ext_send_packet);
}

// static void demo_show_receive(void)
// {
// 	for(int i=0;i<8;i++)
// 	{
// 		ReceivePacket demo_packet;
		
// 		demo_packet.header=0x5A;
// 		demo_packet.detect_color=0;
// 		demo_packet.reset_tracker=0;
// 		demo_packet.reserved=0;
// 		demo_packet.roll=usb_buf[3+6*i];
// 		demo_packet.pitch=usb_buf[4+6*i];
// 		demo_packet.yaw=usb_buf[5+6*i];
// 		demo_packet.aim_x=usb_buf[0+6*i];
// 		demo_packet.aim_y=usb_buf[1+6*i];
// 		demo_packet.aim_z=usb_buf[2+6*i];
		
// 		send(&demo_packet);
		
// 		osDelay(1);
// 	}
// }

// static void usb_printf(const char *fmt,...)
// {
//     static va_list ap;
//     uint16_t len = 0;

//     va_start(ap, fmt);

//     len = vsprintf((char *)usb_buf, fmt, ap);

//     va_end(ap);


//     CDC_Transmit_FS(usb_buf, len);
// }
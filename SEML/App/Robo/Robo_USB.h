/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      no action.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
	*  V2.0.0			Jan-4-2024			HansonZhang			2. PC communication and  auto aiming capability
	*	 V2.0.1			Jan-5-2024			HansonZhang			3. bug fixed: PC communication failed when auto aiming enabled
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#ifndef USB_TASK_H
#define USB_TASK_H

#include "stdint.h"
#include "reference.h"
#include "Robo_Chassis.h"
//#include "struct_typedef.h"

#define AUTO_AIMING_HEADER 0xA5
#define NAVIGATION_HEADER 0x11
#define EXT_HEADER 0x68

typedef struct __attribute__((packed))
{
  uint8_t header;
  _Bool tracking : 1;
  // uint8_t id : 3;          // 0-outpost 6-guard 7-base
  // uint8_t armors_num : 3;  // 2-balance 3-outpost 4-normal
  // uint8_t reserved : 1;
  // float x;
  // float y;
  // float z;
  // float yaw;
  // float vx;
  // float vy;
  // float vz;
  // float v_yaw;
  // float r1;
  // float r2;
  // float dz;
  float pitch;
  float yaw;
  uint8_t fire;
  uint8_t fric_on;
  uint16_t checksum;
}SendPacket;      // Send by PC

typedef struct __attribute__((packed))
{
  uint8_t header;
  float linear_vx;
	float linear_vy;
	float linear_vz;
	float angular_x;
	float angular_y;
	float angular_z;
  uint16_t checksum;
}SendNavi;

typedef struct __attribute__((packed))
{
  uint8_t header;
  uint8_t detect_color : 1;  // 0-red 1-blue
  _Bool reset_tracker : 1;
  uint8_t reserved : 6;
  float yaw;    // rad
  float pitch;  // rad
  float roll;   // rad
  float yaw_odom;
  float pitch_odom;
  float yaw_vel;    // rad/s
  float pitch_vel;  // rad/s
  float roll_vel;   // rad/s	
  float aim_x;
  float aim_y;
  float aim_z;
  uint8_t robot_id;
  uint16_t checksum;
} ReceivePacket;    // Received by PC

typedef struct __attribute__((packed))
{
  uint8_t header;

  uint8_t robot_id;

  uint16_t checksum;
} EXTReceivePacket;    // Received by PC

typedef struct
{
	float vx;
	float vy;
	float vw;
}navi;

extern navi navigation;

extern SendPacket get_received_packet(void);
extern void set_send_packet(ReceivePacket *packet);

extern void USB_Init();
extern void USB_Task(void *conifg);

#endif

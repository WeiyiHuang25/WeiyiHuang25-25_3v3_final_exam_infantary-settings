/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       auto_aiming_task.c/h
  * @brief      Get auto aiming parameters, solve trajectory, then offer feedback to gimbal aiming.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-3-2024     HansonZhang     1. done
	*	 V1.0.1			Jan-5-2024		 HansonZhang		 2. bug fixed: PC communication failed when auto aiming enabled
	*	 V1.0.2			Jan-19-2024		 HansonZhang		 3. bug fixed: added auto aiming control speed restriction
  *
  @verbatim
	@endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include <math.h>
#include <stdio.h>
#include "stdint.h"
#include "main.h"
#include "Robo_USB.h"
#include "PID.h"
#include "Dji_Motor.h"
#include "EzTuner.h"
#include "Robo_gimbal.h"
#include "Robo_chassis.h"


#ifndef __AUTOAIMING_TASK_H__
#define __AUTOAIMING_TASK_H__

//Auto aiming defines
#ifndef PI
#define PI 3.1415926536f
#endif
// #define GRAVITY 9.78
// #define VELOCITY 29.6f
// #define K 0.092f	//or K1 0.038f
// #define BIAS_TIME 100	//In ms, 由于预瞄无法做到完全精准，输入偏置时间
// #define S_BIAS 0.0087	//枪口距离车身中心的距离
// #define Z_BIAS 0.0	//yaw轴电机到枪口水平面的垂直距离

// #define HEADER 0x5A
// #define DETECT_COLOR 0	// 0-red 1-blue

typedef struct
{
	//Output to Bot
	float expect_delta_yaw;
	float expect_delta_pitch;
	
	//Output to PC
	ReceivePacket send;
	
	//Input from PC
	SendPacket receive;
	
	//PID
	PIDConfig_t* yaw_pid;
	PIDConfig_t* pitch_pid;
	
}auto_aiming_control_t;


typedef unsigned char uint8_t;
enum ARMOR_ID
{
    ARMOR_OUTPOST = 0,
    ARMOR_HERO = 1,
    ARMOR_ENGINEER = 2,
    ARMOR_INFANTRY3 = 3,
    ARMOR_INFANTRY4 = 4,
    ARMOR_INFANTRY5 = 5,
    ARMOR_GUARD = 6,
    ARMOR_BASE = 7
};

enum ARMOR_NUM
{
    ARMOR_NUM_BALANCE = 2,
    ARMOR_NUM_OUTPOST = 3,
    ARMOR_NUM_NORMAL = 4
};

enum BULLET_TYPE
{
    BULLET_17 = 0,
    BULLET_42 = 1
};


//设置参数
struct SolveTrajectoryParams
{
    float k;             //弹道系数

    //自身参数
    enum BULLET_TYPE bullet_type;  //自身机器人类型 0-步兵 1-英雄
    float current_v;      //当前弹速
    float current_pitch;  //当前pitch
    float current_yaw;    //当前yaw

    //目标参数
    float pitch;
    float yaw;
    uint8_t fire;
    uint8_t fric_on;

    int bias_time;        //由于预瞄无法做到完全精准，输入偏置时间
    float s_bias;         //枪口前推的距离
    float z_bias;         //yaw轴电机到枪口水平面的垂直距离
    enum ARMOR_ID armor_id;     //装甲板类型  0-outpost 6-guard 7-base
                                //1-英雄 2-工程 3-4-5-步兵 
    enum ARMOR_NUM armor_num;   //装甲板数字  2-balance 3-outpost 4-normal
};

//用于存储目标装甲板的信息
struct tar_pos
{
    float x;           //装甲板在世界坐标系下的x
    float y;           //装甲板在世界坐标系下的y
    float z;           //装甲板在世界坐标系下的z
    float yaw;         //装甲板坐标系相对于世界坐标系的yaw角
};
//单方向空气阻力模型
extern float monoDirectionalAirResistanceModel(float s, float v, float angle);
//完全空气阻力模型
extern float completeAirResistanceModel(float s, float v, float angle);
//pitch弹道补偿
extern float pitchTrajectoryCompensation(float s, float y, float v);
//根据最优决策得出被击打装甲板 自动解算弹道
extern void autoSolveTrajectory(uint32_t inAutoAiming);

//static void demo_traking(_Bool isAutoAiming);

extern float get_auto_aiming_yaw(void);
extern float get_auto_aiming_pitch(void);

extern auto_aiming_control_t  auto_aiming_control;

/**
  * @brief          auto_aiming task, created by main function
  * @param[in]      none
  * @retval         none
  */
extern void AA_Init();
extern void AA_Task(void *conifg);

#endif /*__AUTOAIMING_TASK_H__*/
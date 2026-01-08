#include "Robo_Shoot.h"
Task_Shoot_t shoot;
void Shoot_Open_Firc(float expect_speed);
void Shoot_Close_Firc(void);
void Shoot_Open_Fire(float fire_rate);
void Shoot_Close_Fire(void);
void Shoot_Recover(void);

/**
 * @brief 发射机构任务初始化
 * 该函数执行发射机构任务相关的初始化操作
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Shoot_Init(void)
{
	static Dji_Motor_t fric_L_motor, fric_R_motor, fric_M_motor, dial_motor;
	static SISO_Controller_t fric_L_s_controller, fric_R_s_controller, dial_s_controller;
	float temp;
	// 绑定电机
	shoot.fric_L.motor = DJI_Motor_Init(&fric_L_motor, DJI_Motor_C620, FRIC_L_ID, &FRIC_L_CAN);
	shoot.fric_R.motor = DJI_Motor_Init(&fric_R_motor, DJI_Motor_C620, FRIC_R_ID, &FRIC_R_CAN);
	shoot.dial.motor = DJI_Motor_Init(&dial_motor, DJI_Motor_C610, DIAL_ID, &DIAL_CAN);
	// 绑定电机速度闭环
	fric_L_s_controller.Callback_Fun = Basic_PID_Controller;
	fric_L_s_controller.config = &shoot.fric_L.speed_PID;
	DJI_Motor_Speed_Loop_Init(&fric_L_motor, &fric_L_s_controller);
	fric_R_s_controller.Callback_Fun = Basic_PID_Controller;
	fric_R_s_controller.config = &shoot.fric_R.speed_PID;
	DJI_Motor_Speed_Loop_Init(&fric_R_motor, &fric_R_s_controller);
	dial_s_controller.Callback_Fun = Basic_PID_Controller;
	dial_s_controller.config = &shoot.dial.speed_PID;
	DJI_Motor_Speed_Loop_Init(&dial_motor, &dial_s_controller);
	// 发送电机话题
	// Robo_Push_Message_Cmd("Shoot_frict_L_motor", shoot.fric_L.motor);
	// Robo_Push_Message_Cmd("Shoot_frict_R_motor", shoot.fric_R.motor);
	// Robo_Push_Message_Cmd("Shoot_dial_motor", shoot.dial.motor);
	// temp = 50000;
	// Robo_Push_Message_Cmd("Set_Fric_Speed", temp);
	// 初始化pid
	temp = Get_DJI_Motor_Control_Max(&fric_L_motor);
	PID_Init(&shoot.fric_L.speed_PID, FRIC_KP, FRIC_KI, FRIC_KD, temp, -temp, 0.001f);
	temp = Get_DJI_Motor_Control_Max(&fric_R_motor);
	PID_Init(&shoot.fric_R.speed_PID, FRIC_KP, FRIC_KI, FRIC_KD, temp, -temp, 0.001f);
	temp = Get_DJI_Motor_Control_Max(&dial_motor);
	PID_Init(&shoot.dial.speed_PID, DIAL_Speed_KP, DIAL_Speed_KI, DIAL_Speed_KD, temp, -temp, 0.001f);
	PID_Init(&shoot.dial.position_PID, DIAL_Position_KP, DIAL_Position_KI, DIAL_Position_KD, DIAL_Position_MAXOUT, -DIAL_Position_MAXOUT, 0.001f);

	// 初始化变量
	shoot.dial.expect_angle = Get_DJI_Motor_Position_Data(&dial_motor);
	shoot.status = Shoot_Off;
}

/**
 * @brief 发射机构任务
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Shoot_Task(void *conifg)
{
	float temp,fric_expect_speed;
	uint32_t temp_int;
	// 获取话题
	Robo_Get_Message_Cmd("Set_Fric_Speed", fric_expect_speed);
	// Robo_Get_Message_Cmd("Set_Dial_Speed", shoot.dial.expect_speed);
	Robo_Get_Message_Cmd("Shoot_Mode", temp_int);
	shoot.status = temp_int;
	switch (shoot.status)
	{
	case Shoot_Off:
		Shoot_Close_Fire();
		Shoot_Close_Firc();
		break;
	case Shoot_Single_Shot:
		// 拨盘位置闭环
		shoot.dial.expect_angle += dial_add;
		temp = Zero_Crossing_Process(2 * PI, shoot.dial.expect_angle, Get_Motor_Inc_Position_Data(shoot.dial.motor));
		shoot.dial.expect_speed = Basic_PID_Controller(&shoot.dial.position_PID, temp, Get_Motor_Inc_Position_Data(shoot.dial.motor));
	case Shoot_Fire:
		Shoot_Open_Fire(Fire_Rate);
		Shoot_Open_Firc(Bullet_Speed);
		break;
	case Shoot_Ready:
		Shoot_Close_Fire();
		Shoot_Open_Firc(Bullet_Speed);
		break;
	}
	Robo_Get_Message_Cmd("Recover", temp);
	if(temp)
		Shoot_Recover();
}

static void Shoot_Recover(void)
{
	shoot.dial.expect_speed = 360 * 0.3;
	shoot.dial.send_data = Basic_PID_Controller(&shoot.dial.speed_PID, shoot.dial.expect_speed, Get_Motor_Speed_Data(shoot.dial.motor));
	Motor_Send_Data(shoot.dial.motor, shoot.dial.send_data);
}
/**
 * @brief 开启摩擦轮
 * @param expect_speed 摩擦轮转速
 */
static void Shoot_Open_Firc(float expect_speed)
{
	// 摩擦轮速度闭环
	shoot.fric_L.expect_speed = -expect_speed;
	shoot.fric_L.send_data = Basic_PID_Controller(&shoot.fric_L.speed_PID, shoot.fric_L.expect_speed, Get_Motor_Speed_Data(shoot.fric_L.motor));
	Motor_Send_Data(shoot.fric_L.motor, shoot.fric_L.send_data);
	shoot.fric_R.expect_speed = expect_speed;
	shoot.fric_R.send_data = Basic_PID_Controller(&shoot.fric_R.speed_PID, shoot.fric_R.expect_speed, Get_Motor_Speed_Data(shoot.fric_R.motor));
	Motor_Send_Data(shoot.fric_R.motor, shoot.fric_R.send_data);
}

/**
 * @brief 关闭摩擦轮
 */
static void Shoot_Close_Firc(void)
{
	static uint32_t firc_stop_count;
	if (shoot.fric_L.expect_speed != 0 || shoot.fric_R.expect_speed != 0)
		firc_stop_count = 200;
	if (firc_stop_count > 0)
	{
		firc_stop_count --;
		// 让摩擦轮停下来
		shoot.fric_L.expect_speed = 0;
		shoot.fric_L.send_data = Basic_PID_Controller(&shoot.fric_L.speed_PID, shoot.fric_L.expect_speed, Get_Motor_Speed_Data(shoot.fric_L.motor));
		Motor_Send_Data(shoot.fric_L.motor, shoot.fric_L.send_data);
		shoot.fric_R.expect_speed = 0;
		shoot.fric_R.send_data = Basic_PID_Controller(&shoot.fric_R.speed_PID, shoot.fric_R.expect_speed, Get_Motor_Speed_Data(shoot.fric_R.motor));
		Motor_Send_Data(shoot.fric_R.motor, shoot.fric_R.send_data);
	}
	else
	{
		// 关闭摩擦轮
		shoot.fric_L.send_data = 0;
		Motor_Send_Data(shoot.fric_L.motor, shoot.fric_L.send_data);
		shoot.fric_R.send_data = 0;
		Motor_Send_Data(shoot.fric_R.motor, shoot.fric_R.send_data);
	}
}

float block_pos;
uint16_t block_time = 0;

/**
 * @brief 拨弹盘连发
 * @param fire_rate 射速
 */
static void Shoot_Open_Fire(float fire_rate)
{
	shoot.dial.expect_speed = fire_rate * -360 * 1.5f;
	// 拨盘速度闭环
	if(shoot.dial.expect_speed < shoot.dial.motor->speed - 0.01f)
		block_time++;
	else
	{
		block_time = 0;
	}
	if(block_time == 1000)
		block_pos = shoot.dial.motor->position;
	if(block_time > 1000)
	{
		shoot.dial.expect_speed = fire_rate * 360 * 1.5f;
		if(abs(shoot.dial.motor->position - block_pos)>PI*10/180 || block_time>1000000)
		 	block_time = 0;
	}
	else
		shoot.dial.expect_speed = fire_rate * 360 * 1.5f;
	shoot.dial.send_data = Basic_PID_Controller(&shoot.dial.speed_PID, shoot.dial.expect_speed, Get_Motor_Speed_Data(shoot.dial.motor));
	Motor_Send_Data(shoot.dial.motor, shoot.dial.send_data);
}

/**
 * @brief 关闭拨弹盘
 */
static void Shoot_Close_Fire(void)
{
	static uint32_t fire_stop_count;
	if (shoot.dial.expect_speed != 0)
		fire_stop_count = 100;
	if (fire_stop_count > 0)
	{
		fire_stop_count --;
		shoot.dial.expect_speed = 0;
		shoot.dial.send_data = Basic_PID_Controller(&shoot.dial.speed_PID, 0, Get_Motor_Speed_Data(shoot.dial.motor));
		Motor_Send_Data(shoot.dial.motor, shoot.dial.send_data);
	}
	else
	{
		shoot.dial.send_data = 0;
		Motor_Send_Data(shoot.dial.motor, shoot.dial.send_data);
	}
}
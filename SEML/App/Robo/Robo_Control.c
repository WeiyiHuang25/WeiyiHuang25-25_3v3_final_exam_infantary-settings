#include "Robo_Control.h"
#include "Robo_Common.h"
Task_Control_t control;

__weak void Remote_Control(void)
{
	float temp, temp1, temp2;
	Motor_t *motor;
#define RC_MOVE_MAX 5.0f
#define RC_MOVE_SENSITIVITY (RC_MOVE_MAX / 660.0f)
#define RC_ADD_SENSITIVITY (1.0f / 660.0f)
	// 移动
	temp = -(RC_Ctrl.rc.ch1 - DR16_RC_OFFSET) * RC_MOVE_SENSITIVITY;
	temp1 = (RC_Ctrl.rc.ch0 - DR16_RC_OFFSET) * RC_MOVE_SENSITIVITY;
	// 移动速度归一化
	temp2 = math_sqrt(temp * temp + temp1 * temp1);
	if (temp2 > RC_MOVE_MAX)
	{
		temp2 = RC_MOVE_MAX / temp2;
		temp *= temp2;
		temp1 *= temp2;
	}
	Robo_Push_Message_Cmd("Set_Move_X_Speed", temp);	//Speed in [0,1], in percentage.
	Robo_Push_Message_Cmd("Set_Move_Y_Speed", temp1);
	// 云台yaw跟随
	Robo_Get_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);
	temp -= (RC_Ctrl.rc.ch2 - DR16_RC_OFFSET) * 2 * PI / 1024.0f * 0.001f;
	
	if (temp >= PI)
		temp -= 2 * PI;
	else if (temp < -PI)
		temp += 2 * PI;
	Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);
	// 云台pitch跟随
	Robo_Get_Message_Cmd("Set_Gimbal_Pitch_Angle", temp);
	temp += (RC_Ctrl.rc.ch3 - DR16_RC_OFFSET) * 2 * PI / 1024.0f * 0.001f;
	if (temp >= MAX_Pitch)
		temp = MAX_Pitch;
	else if (temp < MIN_Pitch)
		temp = MIN_Pitch;
	Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", temp);
	// 底盘yaw跟随
	temp = Gimbal_Follow_Offset;
	Robo_Push_Message_Cmd("Set_Chassis_Yaw_Angle", temp);
	Robo_Get_Message_Cmd("Gimbal_Yaw_Motor", motor);
	temp = Get_Motor_Position_Data(motor);
	
	Robo_Push_Message_Cmd("Real_Chassis_Yaw_Angle", temp);

	// S1_UP开启小陀螺
	// if(RC_Ctrl.rc.s1 == 1)
	// {
	// 	if (RC_Ctrl.rc.ch0 > DR16_RC_OFFSET || RC_Ctrl.rc.ch1 > DR16_RC_OFFSET) {
	// 		temp = PI;
	// 		Robo_Push_Message_Cmd("Set_Move_W_Speed", temp);
	// 	} else {
	// 		temp = 1.3f * PI;
	// 		Robo_Push_Message_Cmd("Set_Move_W_Speed", temp);
	// 	}
		
	// }
}

__weak void Robo_Control_Init(void *config)
{
	float temp = 0;
	Robo_Push_Message_Cmd("Set_Move_X_Speed", temp);
	Robo_Push_Message_Cmd("Set_Move_Y_Speed", temp);
	Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);
	Robo_Push_Message_Cmd("Set_Chassis_Yaw_Angle", temp);
	Robo_Push_Message_Cmd("Set_Arm_X_Location",temp);
	temp = AHRS.euler_angle.roll;
	Robo_Get_Message_Cmd("Set_Gimbal_Pitch_Angle", temp);
}
__weak void Robo_Control_Task(void *config)
{
	uint32_t temp;
	float temp_f;
	int flag;
	temp_f = AHRS.euler_angle.yaw;
	Robo_Push_message_Data("Set_Gimbal_Imu_Data", (void *)&AHRS.euler_angle, sizeof(AHRS.euler_angle));
	Robo_Push_Message_Cmd("Real_Chassis_Yaw_Angle", temp_f);
	// S1控制源
	if (RC_Ctrl.rc.s1 == RC_S_DOWM) {
		control.remote_source = Scource_None;
		temp = 0;
		robo_control_flag.remote_off = 1;
		Robo_Push_Message_Cmd("Set_Move_X_Speed", temp);
		Robo_Push_Message_Cmd("Set_Move_Y_Speed", temp);
		temp = Shoot_Off;
		Robo_Push_Message_Cmd("Shoot_Mode", temp);
		flag = 0;
		Robo_Push_Message_Cmd("Set_AA_on", flag);

	// } else if (RC_Ctrl.rc.s1 == RC_S_MID || RC_Ctrl.rc.s1 == RC_S_UP) {
	} else if (RC_Ctrl.rc.s1 == RC_S_MID) {
		control.remote_source = Scource_Remote;
		robo_control_flag.remote_off = 0;
		Remote_Control();
		flag = 0;
		Robo_Push_Message_Cmd("Set_AA_on", flag);
		// s2模式
		switch (RC_Ctrl.rc.s2)
		{
		case RC_S_DOWM:
			temp = Shoot_Off;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			flag = 0;
			Robo_Push_Message_Cmd("Set_AA_on", flag);
			break;
		case RC_S_MID:
			temp = Shoot_Ready;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			flag = 0;
			Robo_Push_Message_Cmd("Set_AA_on", flag);
			break;
		case RC_S_UP:
			temp = Shoot_Fire;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			flag = 0;
			Robo_Push_Message_Cmd("Set_AA_on", flag);
			break;
		default:
			break;
		}
	} else if (RC_Ctrl.rc.s1 == RC_S_UP) {
		control.remote_source = Scource_Remote;
		robo_control_flag.remote_off = 0;
		Remote_Control();
		switch (RC_Ctrl.rc.s2)
		{
		case RC_S_DOWM:
			temp = Shoot_Off;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			break;
		case RC_S_MID:
			temp = Shoot_Ready;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			break;
		case RC_S_UP:
			temp = Shoot_Fire;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
			break;
		default:
			break;
		}
		flag = 1;
	    Robo_Push_Message_Cmd("Set_AA_on", flag);
		//MyRegister();
		// Chassis_KEY_Ctrl();
		// GIMBAL_KEY_Ctrl();
		/*temp = Shoot_Ready;
		Robo_Push_Message_Cmd("Shoot_Mode", temp);
		if(keyboard.mouse_press_left.status == Press_Hold)
		{
			temp = Shoot_Fire;
			Robo_Push_Message_Cmd("Shoot_Mode", temp);
		// }
		// else if(keyboard.mouse_press_left.status == Single_Clink)
		// {
		// 	temp = Shoot_Single_Shot;
		// 	Robo_Push_Message_Cmd("Shoot_Mode", temp);
		// }*/
		// // //Auto_Control();
		// // // s2模式
		// // switch (RC_Ctrl.rc.s2)
		// // {
		// // case RC_S_DOWM:
		// // 	temp = Shoot_Off;
		// // 	Robo_Push_Message_Cmd("Shoot_Mode", temp);
		// // 	break;
		// // case RC_S_MID:
		// // 	//Auto_Control();
		// // 	temp = Shoot_Off;
		// // 	Robo_Push_Message_Cmd("Shoot_Mode", temp);
		// // 	break;
		// // case RC_S_UP:
		// 	// 鼠标左键模式
		
		// 	if(RC_Ctrl.mouse.press_l)
		// 	{
		// 		temp = Shoot_Fire;
		// 		Robo_Push_Message_Cmd("shoot_mode", temp);
		// 	}
		// 	else
		// 	{
		// 		temp = Shoot_Ready;
		// 		Robo_Push_Message_Cmd("shoot_mode", temp);
		// 	}
		// // 	break;
		// // default:
		// // 	break;
		// // }
		// //鼠标右键模式
		// if(RC_Ctrl.mouse.press_r)
		// {
		// 	flag = 1;
		// 	Robo_Push_Message_Cmd("Set_AA_on", flag);
		// }
		// else
		// {
		// 	Robo_Get_Message_Cmd("Set_AA_on", temp);
		// 	if(temp == 1)
		// 	{
		// 		Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", AHRS.euler_angle.yaw);
		// 		Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", AHRS.euler_angle.pitch);
		// 	}
		// 	flag = 0;
		// 	Robo_Push_Message_Cmd("Set_AA_on", flag);
		// }
	}
}

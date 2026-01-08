#include "Robo_Gimbal.h"
Task_Gimbal_t gimbal;

float test_yaw, test_pit;
float test_imu_pitch, test_exp_pitch_angle, test_send_pitch, test_send_yaw, test_noCrosingZero_exp_pitc, test_corr, test_temp2, test_exp_speed;

/**
 * @brief 云台任务初始化
 * 该函数执行云台任务相关的初始化操作
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Gimbal_Init(void)
{
	static Dji_Motor_t pitch_motor,yaw_motor;
	static SISO_Controller_t yaw_s_controller,pitch_s_controller;
	float temp;
	// 绑定电机
	gimbal.pitch.motor = DJI_Motor_Init(&pitch_motor, DJI_Motor_GM6020, Pitch_ID, &Pitch_CAN);
	gimbal.yaw.motor = DJI_Motor_Init(&yaw_motor, DJI_Motor_GM6020, Yaw_ID, &Yaw_CAN);
	// 发送电机话题
	Robo_Push_Message_Cmd("Gimbal_Pitch_Motor",gimbal.pitch.motor);
	Robo_Push_Message_Cmd("Gimbal_Yaw_Motor",gimbal.yaw.motor);
	// 初始化pid
	temp = Get_DJI_Motor_Control_Max(&pitch_motor);
	PID_Init(&gimbal.pitch.speed_PID, Pitch_Speed_KP, Pitch_Speed_KI, Pitch_Speed_KD, temp, -temp, 0.001f);
	PID_Init(&gimbal.pitch.position_PID, Pitch_Position_KP, Pitch_Position_KI, Pitch_Position_KD, Pitch_Position_MAXOUT, -Pitch_Position_MAXOUT, 0.001f);

	temp = Get_DJI_Motor_Control_Max(&yaw_motor);
	PID_Init(&gimbal.yaw.speed_PID, Yaw_Speed_KP, Yaw_Speed_KI, Yaw_Speed_KD, temp, -temp, 0.001f);
	PID_Init(&gimbal.yaw.position_PID, Yaw_Position_KP, Yaw_Position_KI, Yaw_Position_KD, Yaw_Position_MAXOUT, -Yaw_Position_MAXOUT, 0.001f);

	// 初始化变量
	gimbal.pitch.expect_angle = AHRS.euler_angle.pitch;
	gimbal.yaw.expect_angle = AHRS.euler_angle.yaw;
}
/**
 * @brief 云台任务
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Gimbal_Task(void *conifg)
{
	float temp,temp1,temp2, corrected_imu_pitch;
	static float power;
	// 获取话题
	Robo_Get_Message_Cmd("Set_Gimbal_Yaw_Angle", gimbal.yaw.expect_angle);
	Robo_Get_Message_Cmd("Set_Gimbal_Pitch_Angle", gimbal.pitch.expect_angle);
	Robo_Get_message_Data("Set_Gimbal_Imu_Data", (void*)&gimbal.imu);
	if (robo_control_flag.remote_off)
	{
		Motor_Send_Data(gimbal.yaw.motor,0);
		Motor_Send_Data(gimbal.pitch.motor,0);
		return;
	}

	// yaw闭环
	temp = Zero_Crossing_Process(2 * PI, gimbal.yaw.expect_angle + PI, gimbal.imu->yaw + PI);
	Robo_Get_Message_Cmd("Set_AA_on", temp1);
	if(temp1)
	{
		gimbal.yaw.expect_speed = Basic_PID_Controller(&gimbal.yaw.position_PID, temp, gimbal.imu->yaw + PI);
		gimbal.yaw.send_data = Basic_PID_Controller(&gimbal.yaw.speed_PID,gimbal.yaw.expect_speed,Get_Motor_Speed_Data(gimbal.yaw.motor));
	}
	else
	{
		gimbal.yaw.expect_speed = Basic_PID_Controller(&gimbal.yaw.position_PID, temp, gimbal.imu->yaw + PI);
		gimbal.yaw.send_data = Basic_PID_Controller(&gimbal.yaw.speed_PID,gimbal.yaw.expect_speed,Get_Motor_Speed_Data(gimbal.yaw.motor));
	}
	Motor_Send_Data(gimbal.yaw.motor,gimbal.yaw.send_data);
	
	
	test_send_yaw = gimbal.yaw.send_data;

	
	// pitch闭环
	
	
	corrected_imu_pitch = gimbal.imu->pitch > 0 ? gimbal.imu->pitch : 2 * PI + gimbal.imu->pitch; // correct imu pitch
	
	
	// temp = Zero_Crossing_Process(2 * PI, gimbal.pitch.expect_angle + PI, corrected_imu_pitch);
	temp = gimbal.pitch.expect_angle + PI;
    test_noCrosingZero_exp_pitc = gimbal.pitch.expect_angle;
	test_exp_pitch_angle=temp;
	test_imu_pitch=gimbal.imu->pitch;
	test_corr = corrected_imu_pitch;
	
	
	Robo_Get_Message_Cmd("Set_AA_on", temp1);
	if(temp1)
	{
		gimbal.pitch.expect_speed = Basic_PID_Controller(&gimbal.pitch.position_PID, temp, corrected_imu_pitch);
		temp2 = (-9e-08f * gimbal.pitch.expect_angle + 0.0002f) * gimbal.pitch.expect_angle + 0.6147f;
		gimbal.pitch.send_data = Basic_PID_Controller(&gimbal.pitch.speed_PID,gimbal.pitch.expect_speed,Get_Motor_Speed_Data(gimbal.pitch.motor)) +temp2;
	}
	else
	{
		gimbal.pitch.expect_speed = Basic_PID_Controller(&gimbal.pitch.position_PID, temp, corrected_imu_pitch);
		temp2 = (-9e-08f * gimbal.pitch.expect_angle + 0.0002f) * gimbal.pitch.expect_angle + 0.6147f;
		gimbal.pitch.send_data = Basic_PID_Controller(&gimbal.pitch.speed_PID,gimbal.pitch.expect_speed,Get_Motor_Speed_Data(gimbal.pitch.motor)) +temp2;
	}
	
	test_temp2 = temp2;
	test_exp_speed = gimbal.pitch.expect_speed;
	
	Motor_Send_Data(gimbal.pitch.motor, gimbal.pitch.send_data);
	
	test_yaw = gimbal.yaw.expect_speed;
	test_pit = gimbal.pitch.expect_speed;
	test_send_pitch = gimbal.pitch.send_data;
	

	// 发送数据
	Robo_Push_Message_Cmd("Gibmal_Yaw_Vel", gimbal.yaw.expect_speed);
	Robo_Push_Message_Cmd("Gibmal_Pitch_Vel", gimbal.pitch.expect_speed);
}
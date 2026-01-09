#include "Robo_Chassis.h"
#include "Robo_Common.h"
Task_Chassis_t chassis;

float Car_X;
float Car_Y;
float Car_W;
float FL;
float FR;
float BL;
float BR;

/**
 * @brief 将运动速度从云台坐标系转换为底盘坐标系
 * @param old_coordinate_velocity 云台坐标系速度
 * @param expect_angle 期望角(云台角)
 * @param real_angle 实际角(底盘角)
 * @return 底盘坐标系结果
 */

Chassis_Velocity_t coordinate_conversion(Chassis_Velocity_t *old_coordinate_velocity, float expect_angle, float real_angle);
/**
 * @brief 底盘任务初始化
 * 该函数执行底盘任务相关的初始化操作
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Chassis_Init(void)
{
	static Dji_Motor_t chassis_driver_motor[4];
	int16_t temp;
	// 底盘运动学解算初始化
	Chassis_Mechanical_Param_t mechanical_param;
	mechanical_param.wheel_base = Wheel_Base;
	mechanical_param.wheel_track = Wheel_Track;
	mechanical_param.wheel_radius = Wheel_Radius;
	mechanical_param.wheel_motor_reduction_ratio = Motor_Gear_Ratio;	//Motor Gear Ratio
	mechanical_param.rotation_direction = Rotation_Direction;	//+1 for clockwise, -1 for counterclockwise
	Four_Omni_Wheel_Kinematics_Init(&chassis.solve, mechanical_param);
	// 绑定底盘电机
	chassis.driver[CHASSIS_FL].motor = DJI_Motor_Init(&chassis_driver_motor[0], DJI_Motor_C620, FL_ID, &FL_CAN);
	chassis.driver[CHASSIS_FR].motor = DJI_Motor_Init(&chassis_driver_motor[1], DJI_Motor_C620, FR_ID, &FR_CAN);
	chassis.driver[CHASSIS_BL].motor = DJI_Motor_Init(&chassis_driver_motor[2], DJI_Motor_C620, BL_ID, &BL_CAN);
	chassis.driver[CHASSIS_BR].motor = DJI_Motor_Init(&chassis_driver_motor[3], DJI_Motor_C620, BR_ID, &BR_CAN);
	// PID初始化
	temp = Get_DJI_Motor_Control_Max(chassis_driver_motor);
	PID_Init(&chassis.driver[CHASSIS_FL].speed_PID, Chassis_KP, Chassis_KI, Chassis_KD, temp, -temp, 0.001f);
	PID_Init(&chassis.driver[CHASSIS_FR].speed_PID, Chassis_KP, Chassis_KI, Chassis_KD, temp, -temp, 0.001f);
	PID_Init(&chassis.driver[CHASSIS_BL].speed_PID, Chassis_KP, Chassis_KI, Chassis_KD, temp, -temp, 0.001f);
	PID_Init(&chassis.driver[CHASSIS_BR].speed_PID, Chassis_KP, Chassis_KI, Chassis_KD, temp, -temp, 0.001f);
	temp = PI;
	PID_Init(&chassis.yaw_follow_PID, Gimbal_Follow_KP, Gimbal_Follow_KI, Gimbal_Follow_KD, temp, -temp, 0.001f);
	PID_Init(&chassis.buffer_PID, Power_Control_KP, Power_Control_KI, Power_Control_KD, Power_Control_MAXOUT, -Power_Control_MAXOUT, 0.001f);

	//Init Referee
	Reference_Init(&chassis.referee,&huart6);
}

float max_power, realtime_power, power_buffer;
float sum_data_ratio = 0;
float send_data_sum = 0;

float test_send_data[4];
float test_follow_yaw, test_real_yaw;

extern float input_power;

/**
 * @brief 底盘任务
 * @note 该函数定义为弱函数，可以依此为模板定义新的同名函数替换
 */
__weak void Chassis_Task(void *conifg)
{
	static float follow_yaw_angle, real_yaw_angle;
	static float previous_vx = 0, previous_vy = 0;
	Chassis_Velocity_t velocity, filtered_velocity;
	float temp, temp2, temp_array[4];
	float alpha = 0.2;
	float temp_vx = 0, temp_vy = 0;
	int roll;
	
	// 获取话题
	Robo_Get_Message_Cmd("Set_Move_X_Speed", velocity.vx);
	Robo_Get_Message_Cmd("Set_Move_Y_Speed", velocity.vy);
	Robo_Get_Message_Cmd("Set_Move_W_Speed", velocity.vw);
	Robo_Get_Message_Cmd("Set_Chassis_Yaw_Angle", follow_yaw_angle);
	Robo_Get_Message_Cmd("Real_Chassis_Yaw_Angle", real_yaw_angle);
	Robo_Get_Message_Cmd("Chassis_Mode", chassis.mode);
	Robo_Get_Message_Cmd("Set_Roll", roll);
	test_follow_yaw = follow_yaw_angle;
	test_real_yaw = real_yaw_angle;
	// 功率限制
	//max_power = chassis.referee.game_robot_status.robot_level;//chassis_power_limit;
	//max_power = 100;
	max_power = input_power;
	
	//realtime_power = chassis.referee.power_heat_data.chassis_power;
	//power_buffer = chassis.referee.power_heat_data.chassis_power_buffer;
	
	// 处理数据到0~360
	follow_yaw_angle = follow_yaw_angle;
	real_yaw_angle = real_yaw_angle;
	// 将运动速度从云台坐标系转换为底盘坐标系
	velocity.vw = 0;
	chassis.expect_speed = coordinate_conversion(&velocity,follow_yaw_angle,real_yaw_angle);
	//chassis.expect_speed = velocity;
	// 获取旋转方式
	//if(RC_Ctrl.rc.s1 == 1 || RC_Ctrl.keyboard.key_Shift == 1)
	if(roll != 0)
	{
		if (roll == 1) {
			chassis.expect_speed.vw = 2.0f * PI;
		} else if(roll == 2){
			chassis.expect_speed.vw = 5.0f * PI;
		}
		//chassis.expect_speed.vw *= ((0.575f/70)*(max_power-50) + 0.625);
	} else	//Chassis Follow Yaw
	{
		temp = Zero_Crossing_Process(2 * PI, follow_yaw_angle, real_yaw_angle);
		chassis.expect_speed.vw = -Basic_PID_Controller(&chassis.yaw_follow_PID, temp, real_yaw_angle);
	}

	if(abs(chassis.expect_speed.vw) >= 1){
		//chassis.expect_speed.vx *= (80/max_power)*((0.365f/70)*(max_power-50) + 0.4625);
		//chassis.expect_speed.vy *= (80/max_power)*((0.365f/70)*(max_power-50) + 0.4625);
	}
	
	// FD kinematic
	// chassis.expect_speed.vx *= max_power / 100;
	// chassis.expect_speed.vy *= max_power / 100;
	// chassis.expect_speed.vw *= max_power / 80;
	//chassis.expect_speed.vx *= max_power / 80;
	//chassis.expect_speed.vy *= max_power / 80;
    Chassis_Inverse_Kinematics(&chassis.solve, &chassis.expect_speed, chassis.expect_driver_speed);

    // PID Calc
    for (int i = 0; i < 4; i++)
    {
        if (robo_control_flag.remote_off == 0)
        {
            float motor_speed = Get_Motor_Speed_Data(chassis.driver[i].motor);
            chassis.driver[i].send_data = Basic_PID_Controller(&chassis.driver[i].speed_PID, chassis.expect_driver_speed[i], motor_speed);
        }
        else
            chassis.driver[i].send_data = 0;
    }	
	
#include "chassis_power_control.h"
	float motor_send_data[4];
	// power_control(&chassis, motor_send_data, send_data_sum);
	
	send_data_sum = 0;
	for (int i = 0; i < 4; i++)
	{
		Motor_Send_Data(chassis.driver[i].motor, chassis.driver[i].send_data);
		//Motor_Send_Data(chassis.driver[i].motor, motor_send_data[i]);
		send_data_sum += abs(motor_send_data[i]);
	}
	// sum_data_ratio = realtime_power / send_data_sum;
	sum_data_ratio = 0.01;
	send_data_sum *= sum_data_ratio;
	
	// Test
	test_send_data[0] = chassis.driver[0].send_data;
	test_send_data[1] = chassis.driver[1].send_data;
	test_send_data[2] = chassis.driver[2].send_data;
	test_send_data[3] = chassis.driver[3].send_data;
}

/**
 * @brief 将运动速度从云台坐标系转换为底盘坐标系
 * @param old_coordinate_velocity 云台坐标系速度
 * @param expect_angle 期望角(云台角)
 * @param real_angle 实际角(底盘角)
 * @return 底盘坐标系结果
 */
static Chassis_Velocity_t coordinate_conversion(Chassis_Velocity_t *old_coordinate_velocity, float expect_angle, float real_angle)
{
	float temp, temp1;
	Chassis_Velocity_t chassis_coordinate_velocity;
	temp1 = -(Zero_Crossing_Process(2 * PI, expect_angle, real_angle) - real_angle);
	temp = old_coordinate_velocity->vx * cos(temp1) - old_coordinate_velocity->vy * sin(temp1);
	Car_X = temp;
	chassis_coordinate_velocity.vy = old_coordinate_velocity->vx * sin(temp1) + old_coordinate_velocity->vy * cos(temp1);
	Car_Y = chassis_coordinate_velocity.vy;
	chassis_coordinate_velocity.vx = temp;
	chassis_coordinate_velocity.vw = old_coordinate_velocity->vw;
	return chassis_coordinate_velocity;
}


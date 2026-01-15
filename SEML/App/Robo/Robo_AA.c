/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      usb outputs the error message.usbÊä³ö´íÎóÐÅÏ¢
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     -     			CodeAlan        1. solve trajectory
  *  V1.1.0		Jan-2-2024		Xindi Lv		2. complete air resistance model
  *	 V2.0.0		Jan-4-2024		HansonZhang		3. done
  *	 V2.0.1		Jan-5-2024		HansonZhang		4. bug fixed: PC communication failed when auto aiming enabled
  *	 V2.0.2		Jan-19-2024		HansonZhang		5. bug fixed: added restriction to auto aiming control speed
  *	 V3.0.0		Mar-19-2024		HansonZhang		6. deploy in event os
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */


/*
@brief: 弹道解算 适配陈君的rm_vision
@author: CodeAlan  华南师大Vanguard战队
*/
// 近点只考虑水平方向的空气阻力



//TODO 完整弹道模型
//TODO 适配英雄机器人弹道解算


#include <math.h>
#include <stdio.h>

#include "Robo_AA.h"
#include "gpio.h"

#include "main.h"
#include "Robo_Common.h"
#include "Robo_USB.h"
#include "pid.h"

#include "DR16_Remote.h"


#define restriction(input, output, deadline)        \
    {                                                    \
        if ((input) > (deadline)) 											 \
        {                                                \
            (output) = (deadline);                       \
        }                                                \
				else if ((input) < -(deadline)) 								 \
        {                                                \
            (output) = -(deadline);                      \
        }  																							 \
        else                                             \
        {                                                \
            (output) = (input);                          \
        }                                                \
    }

float solve_ramp_exp(float x)
{
    if(x>0)
        return (math_exp(x)-1)/(math_exp(1)-1);
    else
        return -(math_exp(x)-1)/(math_exp(1)-1);
}

struct SolveTrajectoryParams st;
struct tar_pos tar_position[4]; //?????????
float t = 0.5f; // ????

float absolute_pitch = 0.0f, absolute_yaw = 0.0f, expect_delta_pitch = 0.0f, expect_delta_yaw = 0.0f;
float last_non_zero_pitch = 0.0f, last_non_zero_yaw = 0.0f;
float temp;
float yaw_re = 0.0f, pitch_re = 0.0f;
uint8_t fire_status = 0, fric_status = 0;
uint32_t fire_mode_temp;
float loop_constrain(float Input, float minValue, float maxValue);

/*
@brief ???????????
@param s:m ??
@param v:m/s ??
@param angle:rad ??
@return z:m
*/
float monoDirectionalAirResistanceModel(float s, float v, float angle)
{
    float z;
    //t???v?angle??????
    t = (float)((exp(st.k * s) - 1) / (st.k * v * cos(angle)));
    //z???v?angle????
    z = (float)(v * sin(angle) * t - GRAVITY * t * t / 2);
    return z;
}


/*
@brief ??????
@param s:m ??
@param v:m/s ??
@param angle:rad ??
@return z:m
*/
//TODO ??????
float completeAirResistanceModel(float s, float v, float angle)
{
		
    float z;
    float C;
    //time t
    t = (float)((exp(st.k * s) - 1) / (st.k * v * cos(angle)));
    C = atan(sqrt(st.k / GRAVITY) * v * sin(angle)) / sqrt(st.k * GRAVITY);
    if (t < C){//rising
        z = log(cos(sqrt(K * GRAVITY) * (C - t)) / cos(sqrt(K * GRAVITY) * (C))) / K;
    }

    else{//falling
        z = log(1 + K * (v * sin(angle)) * (v * sin(angle)) / GRAVITY) / (2 * K) - GRAVITY * t * t / 2;
    }
    return z;
}

/*
@brief pitch???
@param s:m ??
@param z:m ??
@param v:m/s
@return angle_pitch:rad
*/
float pitchTrajectoryCompensation(float s, float z, float v)
{
    float z_temp = 0.0f, z_actual = 0.0f, dz = 0.0f;
    float angle_pitch;
    int i = 0;
    z_temp = z;
    // iteration
    for (i = 0; i < 20; i++)
    {		
				
        angle_pitch = atan2(z_temp, s); // rad
        z_actual = monoDirectionalAirResistanceModel(s, v, angle_pitch);
				dz = 0.3*(z - z_actual);
        z_temp = z_temp + dz;
        if (fabsf(dz) < 0.00001)
        {
            break;
        }
    }
    return angle_pitch;		
}

/*
@brief ?????????????? ??????
@param inAutoAiming		The condition to enable auto aiming. Release: mouse right button	| Debug: remote control left switch down
*/
void autoSolveTrajectory(uint32_t inAutoAiming)
{

	// 	//debug
	// 	float pitch_data = auto_aiming_control.expect_delta_pitch;
	// 	float yaw_data = auto_aiming_control.expect_delta_yaw;
	// 	float aim_x_data = auto_aiming_control.send.aim_x;
	// 	float aim_y_data = auto_aiming_control.send.aim_y;
	// 	float aim_z_data = auto_aiming_control.send.aim_z;
	
	// 	float *pitch=&pitch_data;
	// 	float *yaw=&yaw_data;
	// 	float *aim_x=&aim_x_data;
	// 	float *aim_y=&aim_y_data;
	// 	float *aim_z=&aim_z_data;
	
    // // ????
    // float timeDelay = st.bias_time/1000.0 + t;
    // st.tar_yaw += st.v_yaw * timeDelay;

    // //??????????
    // //???id??,????????,?????
    // //      2
    // //   3     1
    // //      0
	// 	int use_1 = 1;
	// 	int i = 0;
    // int idx = 0; // ??????
    // //armor_num = ARMOR_NUM_BALANCE ?????
    // if (st.armor_num == ARMOR_NUM_BALANCE) {
    //     for (i = 0; i<2; i++) {
    //         float tmp_yaw = st.tar_yaw + i * PI;
    //         float r = st.r1;
    //         tar_position[i].x = st.xw - r*cos(tmp_yaw);
    //         tar_position[i].y = st.yw - r*sin(tmp_yaw);
    //         tar_position[i].z = st.zw;
    //         tar_position[i].yaw = tmp_yaw;
    //     }

    //     float yaw_diff_min = fabsf(*yaw - tar_position[0].yaw);

    //     //??????? ???????????
    //     float temp_yaw_diff = fabsf(*yaw - tar_position[1].yaw);
    //     if (temp_yaw_diff < yaw_diff_min)
    //     {
    //         yaw_diff_min = temp_yaw_diff;
    //         idx = 1;
    //     }


    // } else if (st.armor_num == ARMOR_NUM_OUTPOST) {  //???
    //     for (i = 0; i<3; i++) {
    //         float tmp_yaw = st.tar_yaw + i * 2.0 * PI/3.0;  // 2/3PI
    //         float r =  (st.r1 + st.r2)/2;   //???r1=r2 ???????
    //         tar_position[i].x = st.xw - r*cos(tmp_yaw);
    //         tar_position[i].y = st.yw - r*sin(tmp_yaw);
    //         tar_position[i].z = st.zw;
    //         tar_position[i].yaw = tmp_yaw;
    //     }

    //     //TODO ??????? ?????????,????????


    // } else {

    //     for (i = 0; i<4; i++) {
    //         float tmp_yaw = st.tar_yaw + i * PI/2.0;
    //         float r = use_1 ? st.r1 : st.r2;
    //         tar_position[i].x = st.xw - r*cos(tmp_yaw);
    //         tar_position[i].y = st.yw - r*sin(tmp_yaw);
    //         tar_position[i].z = use_1 ? st.zw : st.zw + st.dz;
    //         tar_position[i].yaw = tmp_yaw;
    //         use_1 = !use_1;
    //     }

    //     float yaw_diff_min = fabsf(*yaw - tar_position[0].yaw);
    //     for (i = 1; i<4; i++) {
    //         float temp_yaw_diff = fabsf(*yaw - tar_position[i].yaw);
    //         if (temp_yaw_diff < yaw_diff_min)
    //         {
    //             yaw_diff_min = temp_yaw_diff;
    //             idx = i;
    //         }
    //     }

    // }
		
	// auto_aiming_control.send.aim_z = tar_position[idx].z + st.vzw * timeDelay;
	// auto_aiming_control.send.aim_x = tar_position[idx].x + st.vxw * timeDelay;
	// auto_aiming_control.send.aim_y = tar_position[idx].y + st.vyw * timeDelay;

    yaw_re = st.yaw;
    pitch_re = st.pitch;
    fire_status = st.fire;
    fric_status = st.fric_on;
    
	
	//Conditionally start auto aiming
	if(inAutoAiming==1)
	{
        if (auto_aiming_control.receive.tracking==1)
        {
            // 		absolute_pitch = -pitchTrajectoryCompensation(sqrt((aim_x_data) * (aim_x_data) + (aim_y_data) * (aim_y_data)) - st.s_bias, aim_z_data + st.z_bias, st.current_v);
            // //absolute_pitch = math_atan((aim_z_data - st.z_bias)/sqrt((aim_x_data) * (aim_x_data) + (aim_y_data) * (aim_y_data)));
            // absolute_yaw = math_atan2(-st.yw,-st.xw);

            // Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", absolute_yaw);
            // Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", absolute_pitch);

            temp = 1;
            Robo_Push_Message_Cmd("AA Detected", temp);
            if (yaw_re >= PI)
                yaw_re -= 2 * PI;
            else if (yaw_re < -PI)
                yaw_re += 2 * PI;
            Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", yaw_re);

            if (pitch_re >= 0.4f)
                pitch_re = 0.4f;
            else if (pitch_re < -0.55f)
                pitch_re = -0.5f;
            Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", pitch_re);
            switch(fire_status)
            {
                case Shoot_Fire:
                    fire_mode_temp = Shoot_Fire;
                    break;
                case Shoot_Single_Shot:
                    fire_mode_temp = Shoot_Single_Shot;
                    break;
                case Shoot_Ready:
                    fire_mode_temp = Shoot_Ready;
                    break;
                case Shoot_Off:
                    fire_mode_temp = Shoot_Off;
                    break;
                default:
                    fire_mode_temp = Shoot_Off;
                    break;
            }
            Robo_Push_Message_Cmd("Shoot_Mode_temp", fire_mode_temp);
        }
        else
        {
            fire_mode_temp = Shoot_Off;
            Robo_Push_Message_Cmd("Shoot_Mode_temp", fire_mode_temp);
        //     temp = 0;
        //     Robo_Push_Message_Cmd("AA Detected", temp);
        //     Robo_Get_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);
        //     temp += PI;
        //     if (temp >= PI)
        //         temp -= 2 * PI;
        //     else if (temp < -PI)
        //         temp += 2 * PI;
        //     Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);
        }
	}
    else
    {
        absolute_pitch = 0.0f;
        absolute_yaw = 0.0f;
        expect_delta_pitch = 0.0f;
        expect_delta_yaw = 0.0f;
        fire_mode_temp = 0;
        Robo_Push_Message_Cmd("Shoot_Mode_temp", fire_mode_temp);
    }		
}


// 从坐标轴正向看向原点，逆时针方向为正

auto_aiming_control_t  auto_aiming_control;

/**
  * @brief          auto aiming task, created by main function
* @param[in]      	argument: null
  * @retval         none
*/

int AA_on=0;
float startup_front;
Motor_t *motor;
float temp1, temp2;
float odom_yaw, odom_pitch;
float test_pitch, test_yaw;

void AA_Init()
{
    Robo_Get_Message_Cmd("Gimbal_Pitch_Motor", motor);
    temp = Get_Motor_Position_Data(motor);
    temp1 = loop_constrain( -(AHRS.euler_angle.yaw) + YAW_DIRECTION * ( temp - GIMBAL_YAW_OFFSET), -PI, PI);
    odom_yaw = temp1;

    Robo_Get_Message_Cmd("Gimbal_Pitch_Motor", motor);
    temp = Get_Motor_Position_Data(motor);
    temp2 = loop_constrain( -(AHRS.euler_angle.pitch) + PITCH_DIRECTION * ( temp - GIMBAL_PITCH_OFFSET), -PI, PI);
    odom_pitch = temp2;
}

void AA_Task(void *config)
{
		auto_aiming_control.receive = get_received_packet();
		test_pitch = auto_aiming_control.receive.pitch;
        test_yaw = auto_aiming_control.receive.yaw;
		//Set Solve Trajectory Params
		st.k=K;
		st.bullet_type =  BULLET_17;
		st.current_v = VELOCITY;
		
		st.current_pitch = AHRS.euler_angle.pitch;
		st.current_yaw = AHRS.euler_angle.yaw;	
		
		// st.xw = auto_aiming_control.receive.x;
		// st.yw = auto_aiming_control.receive.y;
		// st.zw = auto_aiming_control.receive.z;
		// st.vxw = auto_aiming_control.receive.vx;
		// st.vyw = auto_aiming_control.receive.vy;
		// st.vzw = auto_aiming_control.receive.vz;
		// st.v_yaw = auto_aiming_control.receive.v_yaw;
		// st.tar_yaw = auto_aiming_control.receive.yaw;
		// st.r1 = auto_aiming_control.receive.r1;
		// st.r2 = auto_aiming_control.receive.r2;
		// st.dz = auto_aiming_control.receive.dz;

        st.yaw = auto_aiming_control.receive.yaw;
        st.pitch = auto_aiming_control.receive.pitch;
        st.fire = auto_aiming_control.receive.fire;
        st.fric_on = auto_aiming_control.receive.fric_on;

		st.bias_time = BIAS_TIME;
		st.s_bias = S_BIAS;
		st.z_bias = Z_BIAS;
		st.armor_id = 3;
		st.armor_num = 4;
		
		//Solve Trajectory
    Robo_Get_Message_Cmd("Set_AA_on", AA_on);
		autoSolveTrajectory(AA_on);

        // Get gibmal velocity data
        Robo_Get_Message_Cmd("Gibmal_Yaw_Vel", temp1);
        Robo_Get_Message_Cmd("Gibmal_Pitch_Vel", temp2);
			
		//Upload auto aiming data to PC
		auto_aiming_control.send.header = HEADER;
		auto_aiming_control.send.detect_color = DETECT_COLOR;
		auto_aiming_control.send.reset_tracker = 0;

		auto_aiming_control.send.yaw = AHRS.euler_angle.yaw;
		auto_aiming_control.send.pitch = AHRS.euler_angle.pitch;
		auto_aiming_control.send.roll = AHRS.euler_angle.roll;

        auto_aiming_control.send.yaw_vel = temp1;
        auto_aiming_control.send.pitch_vel = temp2;
        auto_aiming_control.send.roll_vel = 0.0f;

        auto_aiming_control.send.yaw_odom = odom_yaw;
        auto_aiming_control.send.pitch_odom = odom_pitch;

        auto_aiming_control.send.robot_id = chassis.referee.game_robot_status.robot_id;

		set_send_packet(&auto_aiming_control.send);
}


/**
 * @brief 循环限幅函数
 * @param Input 输入值
 * @param minValue 最小值
 * @param maxValue 最大值
 * @return float 输出值
 */
float loop_constrain(float Input, float minValue, float maxValue)
{
    if (maxValue < minValue) {
        return Input;
    }

    if (Input > maxValue) {
        float len = maxValue - minValue;
        while (Input > maxValue) {
            Input -= len;
        }
    } else if (Input < minValue) {
        float len = maxValue - minValue;
        while (Input < minValue) {
            Input += len;
        }
    }
    return Input;
}

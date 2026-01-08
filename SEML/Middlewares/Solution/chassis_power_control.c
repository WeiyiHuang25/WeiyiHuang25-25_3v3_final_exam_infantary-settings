#include "chassis_power_control.h"
#include <stdint.h>
#include "SuperCap.h"

int exceed_limit = 0;

uint8_t cap_state = 0;

int16_t voltage = 0;

uint16_t max_power_limit;

SupCap_s curr_cap;

float initial_power[4];
float initial_total_power=0;
float scale_power[4];
float input_power;

int level = 0;

int level_to_power(uint8_t level)
{
    switch (level)
    {
        case 1:
            return 45;
        case 2:
            return 50;
        case 3:
            return 55;
        case 4:
            return 60;
        case 5:
            return 65;
        case 6:
            return 70;
        case 7:
            return 75;
        case 8:
            return 80;
        case 9:
            return 90;
        case 10:
            return 100;
        default:
            return 45;
    }
}

typedef float fp32;
// void power_control(Task_Chassis_t* chassis, float motor_send_data[4], float realtime_power)//chassis_move_t *power_control)
// {

//     max_power_limit = 40;
//     fp32 chassis_max_power = 0;
//     float initial_give_power[4]; // initial power from PID calculation
//     initial_total_power = 0;
//     fp32 scaled_give_power[4];

//     fp32 chassis_power = 0.0f;
//     fp32 chassis_power_buffer = 0.0f;

//     fp32 torque_coefficient = 1.99688994e-6f; // (20/16384)*(0.3)*(187/3591)/9.55
//     fp32 a = 1.23e-07;						 // k1
//     fp32 k2 = 1.453e-07;					 // k2
//     fp32 constant = 4.081f;

//     power_control_t power_control_hdlr;
//     //power_control_hdlr.max_power = chassis->referee.game_robot_status.chassis_power_limit;
//     // power_control_hdlr.current_power = chassis->referee.power_heat_data.chassis_power;
//     //power_control_hdlr.current_power = realtime_power;
//     //power_control_hdlr.power_buffer = chassis->referee.power_heat_data.chassis_power_buffer;
//     //power_control_hdlr.buffer_pid = chassis->buffer_PID;

// 	//chassis_power = power_control_hdlr.current_power;
// 	//chassis_power_buffer = power_control_hdlr.power_buffer;
// 	//float buffer_pid_out = Basic_PID_Controller(&(power_control_hdlr.buffer_pid), 50, chassis_power_buffer);
// 	// max_power_limit = power_control_hdlr.max_power;
// 	level = chassis->referee.game_robot_status.robot_level;
//     max_power_limit = level_to_power(chassis->referee.game_robot_status.robot_level);
// 	//input_power = max_power_limit - buffer_pid_out;

//     //SupCap_s curr_cap;
//     GetSupCapMeasure(&curr_cap);
// 		voltage = curr_cap.fdb.voltage_cap;
//     if(curr_cap.fdb.voltage_cap<8.0f)
//     {
//         input_power=max_power_limit-4;
//     }
//     else
//     {
//         input_power=max_power_limit+43;
//     }
//     CanCmdSupCap((max_power_limit-4)*100);
		
// 		chassis_max_power = input_power;

//     for (int i = 0; i < 4; i++){
//         // power_control_hdlr.motor_speed[i] = chassis->expect_driver_speed[i];
//         power_control_hdlr.motor_speed[i] = chassis->driver[i].motor->speed;
//         power_control_hdlr.motor_send_data[i] = chassis->driver[i].send_data;
//     }

//     for (uint8_t i = 0; i < 4; i++) // first get all the initial motor power and total motor power
//     {
//         initial_give_power[i] = power_control_hdlr.motor_send_data[i] * torque_coefficient * power_control_hdlr.motor_speed[i] +
//                                 k2 * power_control_hdlr.motor_speed[i] * power_control_hdlr.motor_speed[i] +
//                                 a * power_control_hdlr.motor_send_data[i] * power_control_hdlr.motor_send_data[i] + constant;

//         if (initial_give_power < 0) // negative power not included (transitory)
//             continue;
//         initial_total_power += initial_give_power[i];
//     }

//     if (initial_total_power > chassis_max_power) // determine if larger than max power
//     {
//         fp32 power_scale = chassis_max_power / initial_total_power;
//         for (uint8_t i = 0; i < 4; i++)
//         {
//             scaled_give_power[i] = initial_give_power[i] * power_scale; // get scaled power
//             if (scaled_give_power[i] < 0)
//             {
//                 continue;
//             }

//             fp32 b = torque_coefficient * power_control_hdlr.motor_speed[i];
//             fp32 c = k2 * power_control_hdlr.motor_speed[i] * power_control_hdlr.motor_speed[i] - scaled_give_power[i] + constant;

//             if (power_control_hdlr.motor_send_data[i] > 0) // Selection of the calculation formula according to the direction of the original moment
//             {
//                 fp32 temp = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
//                 if (temp > 16000)
//                 {
//                     power_control_hdlr.motor_send_data[i] = 16000;
//                 }
//                 else
//                     power_control_hdlr.motor_send_data[i] = temp;
//             }
//             else
//             {
//                 fp32 temp = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
//                 if (temp < -16000)
//                 {
//                     power_control_hdlr.motor_send_data[i] = -16000;
//                 }
//                 else
//                     power_control_hdlr.motor_send_data[i] = temp;
//             }
//         }
//     }
//     for (int i = 0; i < 4; i++)
//     {
//         motor_send_data[i] = power_control_hdlr.motor_send_data[i];
//     }
// }

/**
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
  * @file       chassis_power_control.c/h
  * @brief      底盘功率控制
  * @history
  *  Version    Date            Author          Modification
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
  */
// #include "chassis_power_control.h"
// #include "referee.h"
// #include "arm_math.h"
// #include "detect_task.h"
// #include "motor.h"
// #include "usb_debug.h"
// #include "CAN_receive.h"
// #include "CAN_cmd_SupCap.h"
#include <math.h>

#define K1 1.23e-07f
#define K2 1.453e-07f
#define TORQUE_DETA 1.99688994e-6f
#define Constant 4.081f
#define OMEGA_TO_RPM 9.55f
void power_control(Task_Chassis_t* chassis, float motor_send_data[4], float realtime_power)
{ 
  initial_total_power=0;
  //SupCap_s curr_cap;
  GetSupCapMeasure(&curr_cap);
  max_power_limit = level_to_power(chassis->referee.game_robot_status.robot_level);
  //max_power_limit = 120;

  if(curr_cap.fdb.voltage_cap>=8.0f) 
  {
     input_power=max_power_limit+33;
  }
  else
  {
    input_power=max_power_limit-14;
  } 
  CanCmdSupCap((max_power_limit-14)*100);

   for(int i=0;i<4;i++)
   {
    initial_power[i]=chassis->driver[i].send_data*chassis->driver[i].motor->speed*OMEGA_TO_RPM*TORQUE_DETA +K1*chassis->driver[i].send_data*chassis->driver[i].send_data+K2*chassis->driver[i].motor->speed*OMEGA_TO_RPM*chassis->driver[i].motor->speed*OMEGA_TO_RPM+Constant;
    if(initial_power[i]<0)continue;
     initial_total_power+=initial_power[i];
   }

   if(initial_total_power>input_power)  
   {
      float scale=input_power/initial_total_power;
      for(int i=0;i<4;i++)
     {
        scale_power[i]=scale*initial_power[i];
        if(scale_power[i]<0)continue;
        float a=K1;
        float b=TORQUE_DETA*chassis->driver[i].motor->speed*OMEGA_TO_RPM;
        float c=K2*chassis->driver[i].motor->speed*OMEGA_TO_RPM*chassis->driver[i].motor->speed*OMEGA_TO_RPM-scale_power[i]+Constant;
        float inside=b*b-4*a*c;
 
        if(inside<0)
        {
          continue;
        }
        else if(chassis->driver[i].send_data>0)
        {
           float temp=(-b+sqrt(inside))/(2*a);
           if(temp>16000)
           {
             chassis->driver[i].send_data=16000;
           }
           else
           {
             chassis->driver[i].send_data=temp;
           }
        }
        else
        {
         float temp=(-b-sqrt(inside))/(2*a);
         if(temp<-16000)
         {
           chassis->driver[i].send_data=-16000;
         }
         else
         {
             chassis->driver[i].send_data=temp;
         }
        }
     }
   }
  // ModifyDebugDataPackage(1,curr_cap.fdb.voltage_in,"volatage in");
  //  ModifyDebugDataPackage(2,input_power,"input");
  //  ModifyDebugDataPackage(3,*flag,"flag");

}
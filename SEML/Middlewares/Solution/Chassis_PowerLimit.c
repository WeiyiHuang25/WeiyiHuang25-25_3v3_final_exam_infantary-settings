#include "Chassis_PowerLimit.h"

static void Power_Limit_Constrain(float *x, float min, float max)
{
    if (*x > max)
        *x = max;
    else
        *x = min;
}

void Chassis_Power_Limit_Init(Chassis_PowerLimit_t *chassis_power_limit,	Reference_t *reference,SuperCap_t *supercap,float sample_time)
{
	chassis_power_limit->reference = reference;
	chassis_power_limit->supercap = supercap;
	PID_Init(&chassis_power_limit->Power_PID, 0.07f, 0.0003f,0.0f,1.0f,0,sample_time);
}

void Chassis_Power_Limit(Chassis_PowerLimit_t *chassis_power_limit,float send_data[CHASSIS_MOTOR_COUNT])
{
    static float Klimit = 1;
    static float Blimit = 1;
    static float Plimit = 1;
    static float Tlimit = 1;
    static float power_buffer;
    static uint16_t judge_power_max;
		power_buffer = chassis_power_limit->reference->power_heat_data.chassis_power_buffer;
		judge_power_max = chassis_power_limit->reference->game_robot_status.chassis_power_limit;
    // 缓冲能量占比环，总体约束
    if (Check_Reference_Lost(chassis_power_limit->reference) == 0)
    {
        // 裁判系统未失联使用缓冲功率
        if (power_buffer >= 55)
            Blimit = 1;
        else if (power_buffer >= 50)
            Blimit = 0.9f; // 15
        else if (power_buffer >= 40)
            Blimit = 0.75f;
        else if (power_buffer >= 35)
            Blimit = 0.5f;
        else if (power_buffer >= 25)
            Blimit = 0.25f;
        else if (power_buffer >= 15)
            Blimit = 0.125f;
        else if (power_buffer >= 10)
            Blimit = 0.05f;
        else if (power_buffer >= 5)
            Blimit = 0.002f;
        else
            Blimit = 0.0001f;
    }
    else
    {
        // 失联使用0.9倍来限制
        Blimit = 0.9f;
    }
    // 功率限幅环，使用最大转矩电流来限幅
    Tlimit = 1;
    float temp, Im;
    if (power_buffer < 20)
        Im = CHASSIS_DRIVE_IMAX * 0.3f;
    else
        Im = CHASSIS_DRIVE_IMAX;
    for (int i = 0; i < CHASSIS_MOTOR_COUNT; i++)
    {
        if (send_data[i] != 0.0f)
            temp = abs(Im / send_data[i]);
        else
            temp = 1;
        if (Tlimit > temp)
        {
            Tlimit = temp;
        }
    }

    // 功率环
    if (chassis_power_limit->status.enable_supercap == 1)
    {
        // 开启超级电容的矫正函数，配合缓冲环使用
        Plimit = 0.03f * power_buffer + 0.8f;
        Set_SuperCap_CompensationLimit(chassis_power_limit->supercap,200);
				// 缓冲功率低于一定值优先恢复缓存功率
        if (power_buffer < 40)
        {
            Set_SuperCap_PowerLimit(chassis_power_limit->supercap,judge_power_max * 0.4f);
        }
        else
        {
            Set_SuperCap_PowerLimit(chassis_power_limit->supercap,judge_power_max);
        }
        if (Get_SuperCap_Vcap(chassis_power_limit->supercap) < 8.0f)
        {
            // 欠压关闭超级电容模式
            chassis_power_limit->status.enable_supercap = 0;
            Set_SuperCap_CompensationLimit(chassis_power_limit->supercap,0);
            Plimit = 1;
        }
    }
    else
    {
        // 更新超级电容功率
        Set_SuperCap_PowerLimit(chassis_power_limit->supercap,judge_power_max);
        Plimit = 1;
        // 关闭超级电容输出
        Set_SuperCap_CompensationLimit(chassis_power_limit->supercap,0);
        // 超电没失联使用其底盘功率做反馈
        if (Get_SuperCap_Loss_Of_Connection(chassis_power_limit->supercap) == SUCCESS)
        {
            temp = Basic_PID_Controller(&chassis_power_limit->Power_PID, judge_power_max *0.9f, Get_SuperCap_Pchassis(chassis_power_limit->supercap) / 100.0f);
            Power_Limit_Constrain(&temp, 0.01, 1);
            Plimit = temp;
        }
        else if(chassis_power_limit->status.enable_supercap == 1)
        {
            // 如果裁判系统也失联使用0.36倍(0.4*0.9)来限制
            Plimit = 0.4f;
        }
        else
        {
            Plimit = 0.9f;
        }
    }

    Klimit =  Plimit * Tlimit * Blimit;
    Power_Limit_Constrain(&Klimit, 0.2, 1);
    // 等比缩放电流
    for (int i = 0; i < CHASSIS_MOTOR_COUNT; i++)
       send_data[i] *= Klimit;
}

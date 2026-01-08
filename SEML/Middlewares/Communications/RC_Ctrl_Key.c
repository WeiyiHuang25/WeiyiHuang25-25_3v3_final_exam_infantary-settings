#include "RC_Ctrl_Key.h"
Coordinates_t Absolute_Speed; //底盘绝对坐标系速度，一个速度暂存值中间变量
eChassisAction Chassis_Mode = CHASSIS_FOLLOW_GIMBAL;   //默认底盘跟随云台行走
Key_Mode chassic_mode;
extern Task_Gimbal_t gimbal;
extern Keyboard_t keyboard;
extern RC_Ctrl_t RC_Ctrl;
extern Task_Chassis_t chassis;
uint16_t Get_Chassis_Pow_Limit;          //获取功率上限
extern Task_Shoot_t shoot;
int flag=0;
float flag2;
float actual_vx, actual_vy;

float pw_speed(uint16_t pw)
{
	float sp;
	switch(pw)
	{
        case 45:
        case 50:
        case 55:
	        sp = KEY_CHASSIS_MOVE_MAX_45_55W;				
        break;
        case 60:
        case 65:
        case 70:
            sp = KEY_CHASSIS_MOVE_MAX_60_70W;				
            break;
        case 75:
        case 80:
        case 85:
			sp = KEY_CHASSIS_MOVE_MAX_75_95W;								
            break;
        case 90:
        case 95:
		case 100: 
			sp = KEY_CHASSIS_MOVE_MAX_90_100W;			
            break;
        default:
			sp = KEY_CHASSIS_MOVE_MAX_45_55W;		
        break;
	}
	return sp;
}

/**
 * @brief 开自瞄
 */
void Start_AA(void)
{
    int temp = 1;
    Robo_Push_Message_Cmd("Set_AA_on",temp);
}
/**
 * @brief 关自瞄
 */
void Stop_AA(void)
{
    int temp = 0;
    Robo_Push_Message_Cmd("Set_AA_on",temp);
}

void Recover_Shoot(void)
{
    int temp = 1;
    Robo_Push_Message_Cmd("Recover", temp);
}

void Stop_Recover(void)
{
    int temp = 0;
    Robo_Push_Message_Cmd("Recover", temp);
}

/**
 * @brief 速度先缓启动再正常累加
 */
void Front_Press_Hold(void)
{
    uint32_t System_Current_Time;
	uint32_t System_Target_Time;
	System_Current_Time = SEML_GetTick();
    System_Target_Time = System_Current_Time + MOVE_KEY_FB_TIME_MAX;
    // if(System_Current_Time <= System_Target_Time)
    // {
    //     if( IF_SuperCap_OFF )		Absolute_Speed.vx += SLOW_START_SPEED * FBA;				
	// 	else if( IF_SuperCap_ON )	Absolute_Speed.vx += SUPERCAP_SLOW_START_SPEED * FBA;
    // }  
	Absolute_Speed.vx = -5.0f;//Ramp_float(Key_Max, Absolute_Speed.vx,FBA);		
    //Absolute_Speed.vx = Ramp_float(Key_Max, Absolute_Speed.vx,FBA);	
}
void Back_Press_Hold(void)
{
    uint32_t System_Current_Time;
	uint32_t System_Target_Time;
	System_Current_Time = SEML_GetTick();
    System_Target_Time = System_Current_Time + MOVE_KEY_FB_TIME_MAX;
    // if(System_Current_Time <= System_Target_Time)
    // {
    //   	if( IF_SuperCap_OFF )		Absolute_Speed.vx -= SLOW_START_SPEED * FBA;				
	// 	else if( IF_SuperCap_ON )	Absolute_Speed.vx -= SUPERCAP_SLOW_START_SPEED * FBA;	
    // }   
	
    Absolute_Speed.vx = 5.0f;//Ramp_float(-Key_Max, Absolute_Speed.vx,FBA);	
    
    Robo_Push_Message_Cmd("Set_Move_X_Speed", Absolute_Speed.vx);
}
void Left_Press_Hold(void)
{
    uint32_t System_Current_Time;
	uint32_t System_Target_Time;
	System_Current_Time = SEML_GetTick();
    System_Target_Time = System_Current_Time + MOVE_KEY_LR_TIME_MAX;
    // if(System_Current_Time <= System_Target_Time)
    // {
    //   	if( IF_SuperCap_OFF )		Absolute_Speed.vx += SLOW_START_SPEED * LRA;				
	// 	else if( IF_SuperCap_ON )	Absolute_Speed.vx += SUPERCAP_SLOW_START_SPEED * LRA;	
    // }   
    Absolute_Speed.vy = -5.0f;//Ramp_float(Key_Max, Absolute_Speed.vy,LRA);	
}
void Right_Press_Hold(void)
{
    uint32_t System_Current_Time;
	uint32_t System_Target_Time;
	System_Current_Time = SEML_GetTick();
    System_Target_Time = System_Current_Time + MOVE_KEY_LR_TIME_MAX;
    // if(System_Current_Time <= System_Target_Time)
    // {
    //   	if( IF_SuperCap_OFF )		Absolute_Speed.vx -= SLOW_START_SPEED * LRA;				
	// 	else if( IF_SuperCap_ON )	Absolute_Speed.vx -= SUPERCAP_SLOW_START_SPEED * LRA;	
    // } 
    Absolute_Speed.vy = 5.0f;//Ramp_float(-Key_Max, Absolute_Speed.vy,LRA);	
}

/**
 * @brief 按键释放，分级减速
 */
void Front_Press_Release(void)
{
    if (Absolute_Speed.vx > 0)    		
     Absolute_Speed.vx = Ramp_float(0, Absolute_Speed.vx,0.05*Key_Max);
}

void Back_Press_Release(void)
{
    if (Absolute_Speed.vx < 0)    		
     Absolute_Speed.vx = Ramp_float(0, Absolute_Speed.vx,0.05*Key_Max);
     
}
void Left_Press_Release(void)
{
    if(Absolute_Speed.vy > 0)
     Absolute_Speed.vy = Ramp_float(0, Absolute_Speed.vy,0.05*Key_Max);
}
void Right_Press_Release(void)
{
    if(Absolute_Speed.vy < 0)
     Absolute_Speed.vy = Ramp_float(0, Absolute_Speed.vy,0.05*Key_Max);
}

/**
 * @brief 获取裁判系统功率上限
 * @return temp_power 功率上限
 */
uint16_t JUDGE_usGetChassis(void)
{
	uint16_t static temp_power;
	temp_power = 100;//chassis.omni_wheel_chassis_power_limit.reference->game_robot_status.chassis_power_limit
	//Robo_Get_Message_Cmd("Power Limit", temp_power);
	return temp_power;
}

/**
 * @brief 超电模式选择
 */
void Chassis_Mode_Choose(void)
{
	// if (chassis.omni_wheel_chassis_power_limit.status.enable_supercap == DISABLE)
	// {
	// 	chassis.omni_wheel_chassis_power_limit.status.enable_supercap = ENABLE;
	// }
    // else if(chassis.omni_wheel_chassis_power_limit.status.enable_supercap == ENABLE)
	// {		
    //     chassis.omni_wheel_chassis_power_limit.status.enable_supercap = DISABLE;
	// }
}

/**
 * @brief 小陀螺与底盘跟随模式切换
 */
void Mode_Switch(Button_t *button)
{
    // if(Chassis_Mode == CHASSIS_FOLLOW_GIMBAL)
    //     Chassis_Mode = CHASSIS_SPIN;
    // else if(Chassis_Mode == CHASSIS_SPIN)
    //     Chassis_Mode = CHASSIS_FOLLOW_GIMBAL;
    if(keyboard.key_Shift.status == Press_Hold)//.press_count%2 == 1)
    {
        flag = 2;
        Robo_Push_Message_Cmd("Spin", flag);
    }
    else
    {
        flag = 3;
        Robo_Push_Message_Cmd("Spin", flag);
    }
}

/**
 * @brief 底盘键盘模式
 */
void Chassis_KEY_Ctrl(void)
{
	//flag = 1;
    float temp;
    temp = Gimbal_Follow_Offset;
    Motor_t *motor;
	
	Robo_Push_Message_Cmd("Set_Chassis_Yaw_Angle", temp);
    Robo_Get_Message_Cmd("Gimbal_Yaw_Motor", motor);
    temp = Get_Motor_Position_Data(motor);
	Robo_Push_Message_Cmd("Real_Chassis_Yaw_Angle", temp);
    //依据功率上限选择最大速度
    uint16_t Get_Chassis_Pow_Limit = ((Reference_t *)referee)->game_robot_status.chassis_power_limit;
    Robo_Push_Message_Cmd("Chassis_Pow_Limit" , Get_Chassis_Pow_Limit);
    //判断超级电容状态，调整加速度以及最大速度
	switch (DISABLE)
    {
    	case DISABLE:
			FBA = Add_DISABLE_SuperCup_Speed; //加速度
			LRA = Add_DISABLE_SuperCup_Speed; //加速度
    		break;
		
    	case ENABLE:
			FBA = Add_ENABLE_SuperCup_Speed; //加速度
			LRA = Add_ENABLE_SuperCup_Speed; //加速度
    		break;
    	default:
    		break;
    }
    /*
	switch(power_limit_point[0])
	{
        case 45:
        case 50:
        case 55:
	        Key_Max = KEY_CHASSIS_MOVE_MAX_45_55W;				
        break;
        case 60:
        case 65:
        case 70:
            Key_Max = KEY_CHASSIS_MOVE_MAX_60_70W;				
            break;
        case 75:
        case 80:
        case 85:
			Key_Max = KEY_CHASSIS_MOVE_MAX_75_95W;								
            break;
        case 90:
        case 95:
		case 100: 
			Key_Max = KEY_CHASSIS_MOVE_MAX_90_100W;			
            break;
        default:
			Key_Max = KEY_CHASSIS_MOVE_MAX_45_55W;		
        break;
	}*/
	
	
if (power_limit_point[0] >= 45 && power_limit_point[0] <= 55) {  
    Key_Max = KEY_CHASSIS_MOVE_MAX_45_55W;  
} else if (power_limit_point[0] >= 60 && power_limit_point[0] <= 70) {  
    Key_Max = KEY_CHASSIS_MOVE_MAX_60_70W;  
} else if (power_limit_point[0] >= 75 && power_limit_point[0] <= 85) {  
    Key_Max = KEY_CHASSIS_MOVE_MAX_75_95W;  
} else if (power_limit_point[0] >= 90 && power_limit_point[0] <= 100) {  
    Key_Max = KEY_CHASSIS_MOVE_MAX_90_100W;  
} else {  
    Key_Max = KEY_CHASSIS_MOVE_MAX_45_55W;  
}
	Key_Max = 5.0f;
	//flag2 = Key_Max;
    switch(Chassis_Mode) 
    {
		case CHASSIS_FOLLOW_GIMBAL://跟随云台
            chassic_mode = Yaw_Follow;
            Key_Max += 0.3f;
			break;
		case CHASSIS_SPIN:		//小陀螺模式
             chassic_mode = Rotate;
			switch (Get_Chassis_Pow_Limit)
            {
            	case 45:
                case 50:
                case 55:
					temp =- Round_SPEED_1 * Round_S;
                    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);					
            	    break;
                case 60:
            	case 65:
                case 70:
                    temp =- Round_SPEED_2 * Round_S;
                    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);
            		break;
                case 75:
                case 80:
                case 85:
					temp =- Round_SPEED_3 * Round_S;	
                    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);				
            	    break;
                case 90:
                case 95:
				case 100: 
					temp =- Round_SPEED_4 * Round_S;
                    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);
            		break;
            	default:
					temp =- Round_SPEED * Round_S;
                    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);		
            		break;
            }
			break;
		default:
			break;
    }
    //temp =- 1.8f * Round_S;
    Robo_Push_Message_Cmd("Chassis_vw_speed" , temp);	
    //actual_vx = -Absolute_Speed.vx - Absolute_Speed.vy;
    //actual_vy = Absolute_Speed.vx - Absolute_Speed.vy;
  Robo_Push_Message_Cmd("Set_Move_X_Speed", Absolute_Speed.vx);
    Robo_Push_Message_Cmd("Set_Move_Y_Speed", Absolute_Speed.vy);
}

/**
 * @brief 云台键盘模式
 */
void GIMBAL_KEY_Ctrl(void)
{   
    float temp;
    Robo_Get_Message_Cmd("Set_Gimbal_Pitch_Angle", gimbal.pitch.expect_angle);
    temp = gimbal.pitch.expect_angle;
/*     //使用电机刻度限位
	if( (gimbal.pitch.motor->position < Pitch_Limit_High_Angle) && (RC_Ctrl.mouse.y<0) )	
        RC_Ctrl.mouse.y=0;
	else if( (gimbal.pitch.motor->position > Pitch_Limit_Low_Angle) && (RC_Ctrl.mouse.y>0))	
        RC_Ctrl.mouse.y=0;
	else  */
    if (temp >= 0.3f)
		temp = 0.3f;
	else if (temp < -0.3f)
		temp = -0.3f;
	temp += (Mouse_Sensitivit * RC_Ctrl.mouse.y);
    temp = Lowpass(temp, gimbal.pitch.expect_angle, 0.2f);
    //Ramp_float(, Absolute_Speed.vy,0.02f);
    Robo_Push_Message_Cmd("Set_Gimbal_Pitch_Angle", temp);
    Robo_Get_Message_Cmd("Set_Gimbal_Yaw_Angle", gimbal.yaw.expect_angle);
    temp = gimbal.yaw.expect_angle;
	temp -= (Mouse_Sensitivit * RC_Ctrl.mouse.x);
    temp = Lowpass(temp, gimbal.yaw.expect_angle, 0.1f);
    if (temp >= PI)
		temp -= 2 * PI;
	else if (temp < -PI)
		temp += 2 * PI;
    Robo_Push_Message_Cmd("Set_Gimbal_Yaw_Angle", temp);			
}

/**
 * @brief 拨弹盘开启供弹，开始打弹
 */
void Gimbal_Shoot_Fire(Button_t *button)
{
    int temp = Shoot_Fire;
    Robo_Push_Message_Cmd("Shoot_Mode", temp);
}

/**
 * @brief 单发
 */
void Gimbal_Single_Shot(Button_t *button)
{
    int temp = Shoot_Single_Shot;
    Robo_Push_Message_Cmd("Shoot_Mode", temp);
}

/**
 * @brief 摩擦轮开启供弹，准备打弹
 */
void Gimbal_Shoot_Off(Button_t *button)
{
    int temp = Shoot_Ready;
    Robo_Push_Message_Cmd("Shoot_Mode", temp);
}
/*
void MyRegister(void)
{
    if(keyboard.key_W.status == Long_Press_Hold)
		{
			Front_Press_Hold();
		}
		/*
    if(keyboard.key_W.status == Press_None)
		{			
			Front_Press_Release();
			flag = 2;
		}*/
/*
    if(keyboard.key_S.status == Long_Press_Hold)
		{
			Back_Press_Hold();
			//flag = 3;
		}*/
		/*
    if(keyboard.key_S.status == Press_None)
		{
			Back_Press_Release();
			flag = 4;
		}			*//*
    if(keyboard.key_A.status == Long_Press_Hold)
		{
			Left_Press_Hold();
			//flag = 5;
		}*/
		/*
    if(keyboard.key_A.status == Press_None)
		{
      Left_Press_Release();
			flag = 6;
		}*/
		/*
    if(keyboard.key_D.status == Long_Press_Hold)
		{			
			Right_Press_Hold();
			//flag = 7;
		}*/
		/*
    if(keyboard.key_D.status == Press_None)
		{			
			Right_Press_Release();
			flag = 8;
		}*//*
    if(keyboard.mouse_press_left.status == Long_Press_Hold)
		{			
			Gimbal_Shoot_Fire(&keyboard.mouse_press_left);
			//flag = 9;
		}
    if(keyboard.mouse_press_left.status == Single_Clink)
		{
			Gimbal_Single_Shot(&keyboard.mouse_press_left);
			//flag = 10;
		}
    //if(keyboard.mouse_press_right.status == Long_Press_Hold)    Start_AA();
    //if(keyboard.mouse_press_right.status == Press_None)    Stop_AA();
    if(keyboard.key_Shift.status == Single_Clink)
		{    
			Mode_Switch(&keyboard.key_Shift);
			//flag = 11;
		}
}*/

/**
 * @brief 键盘模式初始化
 * @param keyboard 键盘按键句柄
 */
void Key_Init(Keyboard_t*keyboard)
{
    assert_param(keyboard != NULL);
    Button_Init(&keyboard->key_W,1,Get_Key_Status,(void*)RC_Key_W);
    Button_Init(&keyboard->key_S,1,Get_Key_Status,(void*)RC_Key_S);
    Button_Init(&keyboard->key_A,1,Get_Key_Status,(void*)RC_Key_A);
    Button_Init(&keyboard->key_D,1,Get_Key_Status,(void*)RC_Key_D);
    Button_Init(&keyboard->key_Shift,1,Get_Key_Status,(void*)RC_Key_Shift);
    Button_Init(&keyboard->key_Ctrl,1,Get_Key_Status,(void*)RC_Key_Ctrl);
    Button_Init(&keyboard->key_Q,1,Get_Key_Status,(void*)RC_Key_Q);
    Button_Init(&keyboard->key_E,1,Get_Key_Status,(void*)RC_Key_E);
    Button_Init(&keyboard->key_R,1,Get_Key_Status,(void*)RC_Key_R);
    Button_Init(&keyboard->key_F,1,Get_Key_Status,(void*)RC_Key_F);
    Button_Init(&keyboard->key_G,1,Get_Key_Status,(void*)RC_Key_G);
    Button_Init(&keyboard->key_Z,1,Get_Key_Status,(void*)RC_Key_Z);
    Button_Init(&keyboard->key_X,1,Get_Key_Status,(void*)RC_Key_X);
    Button_Init(&keyboard->key_C,1,Get_Key_Status,(void*)RC_Key_C);
    Button_Init(&keyboard->key_V,1,Get_Key_Status,(void*)RC_Key_V);
    Button_Init(&keyboard->key_B,1,Get_Key_Status,(void*)RC_Key_B);  
    Button_Init(&keyboard->mouse_press_left,1,Get_Mouse_Left_Status,(void*)RC_Mouse_Left);
    Button_Init(&keyboard->mouse_press_right,1,Get_Mouse_Right_Status,(void*)RC_Mouse_Right);
}

/**
 * @brief 键盘事件回调注册
 * @param keyboard 键盘按键句柄
 */
void Key_Register(Keyboard_t*keyboard)
{
    //按键按下，速度正常累加
    Button_Register(&keyboard->key_W,Press_Hold,Front_Press_Hold);
    Button_Register(&keyboard->key_S,Press_Hold,Back_Press_Hold);
    Button_Register(&keyboard->key_A,Press_Hold,Left_Press_Hold);
    Button_Register(&keyboard->key_D,Press_Hold,Right_Press_Hold);
    //按键释放，减速
    Button_Register(&keyboard->key_W,Press_None,Front_Press_Release);
    Button_Register(&keyboard->key_S,Press_None,Back_Press_Release);
    Button_Register(&keyboard->key_A,Press_None,Left_Press_Release);
    Button_Register(&keyboard->key_D,Press_None,Right_Press_Release);
    //鼠标左键打弹
    Button_Register(&keyboard->mouse_press_left,Press_Hold,Gimbal_Shoot_Fire);
    Button_Register(&keyboard->mouse_press_left,Press_None,Gimbal_Shoot_Off);
    //鼠标右键自瞄
    //Button_Register(&keyboard->mouse_press_right,Press_Hold,Start_AA);
    //Button_Register(&keyboard->mouse_press_right,Press_None,Stop_AA);
    //开启超电
    //Button_Register(&keyboard->key_C,Single_Clink,Chassis_Mode_Choose);
    //shift按下切换为小陀螺或底盘跟随
    Button_Register(&keyboard->key_Shift,Press_Hold,Mode_Switch);
    Button_Register(&keyboard->key_Shift,Press_None,Mode_Switch);

    Button_Register(&keyboard->key_F,Press_Hold,Recover_Shoot);
    Button_Register(&keyboard->key_F,Press_None,Stop_Recover);
}

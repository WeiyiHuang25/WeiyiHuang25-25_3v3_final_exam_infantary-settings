#ifndef _RC_CTRL_KEY_H_
#define _RC_CTRL_KEY_H_

#include "button.h"
#include "DR16_Remote.h"
#include "Robo_chassis.h"
#include "Robo_gimbal.h"
#include "Robo_Shoot.h"
#include "reference.h"
#include "math_filter.h"

#define IF_SuperCap_ON    (chassis.omni_wheel_chassis_power_limit.status.enable_supercap == ENABLE)
#define IF_SuperCap_OFF   (chassis.omni_wheel_chassis_power_limit.status.enable_supercap == DISABLE)

/*  键盘模式下前进，后退每次增加的速度，ENABLE与DISABLE表示是否开启超级电容  */
#define Add_DISABLE_SuperCup_Speed 	0.05f  
#define Add_ENABLE_SuperCup_Speed 	0.1f  
/* FB：前后，LR：左右 A：加速度 Key_Max: 最大速度 */
static float FBA, LRA, Key_Max;
/*  键盘模式下对应底盘功率下前进，后退的最大速度，ENABLE与DISABLE表示是否开启超级电容  */
#define KEY_CHASSIS_MOVE_MAX_45_55W		1.0f//1.7f
#define KEY_CHASSIS_MOVE_MAX_60_70W		1.2f//2.2f
#define KEY_CHASSIS_MOVE_MAX_75_95W		1.3f//2.6f
#define KEY_CHASSIS_MOVE_MAX_90_100W	1.4f//3.0f
/*  小陀螺转速，RAD/S,6.28=2PI,即初始单位是1圈/s,通过相乘的系数Round_SPEED即可设置多少圈有一秒  */
#define Round_S  2 * PI  
#define Round_SPEED  1.0f
#define Round_SPEED_1  1.0f  
#define Round_SPEED_2  1.3f  
#define Round_SPEED_3  1.5f  
#define Round_SPEED_4  1.6f
/*  Pitch电机刻度限位  */
#define Pitch_Limit_High_Angle 1
#define Pitch_Limit_Low_Angle  1
/*  键盘模式下缓启动的时间阈值，单位ms  */
#define MOVE_KEY_FB_TIME_MAX 1000
#define MOVE_KEY_LR_TIME_MAX 1000
/*  键盘模式下缓启动的加速系数  */
#define SLOW_START_SPEED 0.05f
#define SUPERCAP_SLOW_START_SPEED 0.1f
/*  鼠标灵敏度(越大越灵敏)  */
#define Mouse_Sensitivit 0.0002f

typedef enum
{
	Normal_Mode = 1,
	Rotate = 2,
	Yaw_Follow = 3,
} Key_Mode;

typedef struct 
{
	Button_t  key_W ;
	Button_t  key_S ;
	Button_t  key_A ;
	Button_t  key_D ;
	Button_t  key_Shift ;
	Button_t  key_Ctrl ;
	Button_t  key_Q ;
	Button_t  key_E ;
	Button_t  key_R ;
	Button_t  key_F ;
	Button_t  key_G ;
	Button_t  key_Z ;
	Button_t  key_X ;
	Button_t  key_C ;
    Button_t  key_V ;
	Button_t  key_B ;
	Button_t  mouse_press_left;
	Button_t  mouse_press_right;
	Key_Mode key;
} Keyboard_t;					/**< 键盘 */

typedef struct
{
    float vx;
    float vy;
    float vw;
}Coordinates_t;

typedef enum
{
    CHASSIS_FOLLOW_GIMBAL,	//底盘跟随云台行走
    CHASSIS_SPIN ,		//小陀螺模式
    CHASSIS_NORMAL  ,//底盘不跟随云台行走
    CHASSIS_CORGI   ,//扭腰模式
    CHASSIS_BUFF  ,//打符模式
    CHASSIS_RPPUP  ,//爬坡模式
    CHASSIS_MISS    ,//自动闪避模式
} eChassisAction;

extern Key_Mode chassic_mode;


/**
 * @brief 键盘模式初始化
 * @param keyboard 键盘按键句柄
 */
void Key_Init(Keyboard_t*keyboard);

/**
 * @brief 按键按住，速度正常累加
 */
void Front_Press_Hold(void);
void Back_Press_Hold(void);
void Left_Press_Hold(void);
void Right_Press_Hold(void);

/**
 * @brief 按键释放，分级减速
 */
void Front_Press_Release(void);
void Back_Press_Release(void);
void Left_Press_Release(void);
void Right_Press_Release(void);
void Start_AA(void);
void Stop_AA(void);
extern void Chassis_KEY_Ctrl(void);
extern void MyRegister(void);
void Mode_Switch(Button_t *button);
void Gimbal_Shoot_Fire(Button_t *button);
void Gimbal_Shoot_Off(Button_t *button);
void Gimbal_Single_Shot(Button_t *button);
extern void GIMBAL_KEY_Ctrl(void);
uint16_t JUDGE_usGetChassis(void);
void Chassis_Mode_Choose(void);
#endif
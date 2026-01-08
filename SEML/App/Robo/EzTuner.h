#ifndef _EZTUNER_H_
#define _EZTUNER_H_
#endif

#define infantry 2




// //For infantry 1
// #if infantry==1

// //Robo_Control.c
// #define Gimbal_Follow_Offset 2.7f
// #define MAX_Pitch 0.3f
// #define MIN_Pitch -0.3f



// //Robo_Chassis.c
// //Chassis param
// #define Wheel_Base 385  //in mm
// #define Wheel_Track 371
// #define Wheel_Radius 76

// #define Motor_Gear_Ratio 19

// #define CW 1
// #define CCW -1
// #define Rotation_Direction CW

// //Motor Electric param
// #define FL_ID 3
// #define FL_CAN can1

// #define FR_ID 2
// #define FR_CAN can1

// #define BL_ID 4
// #define BL_CAN can1

// #define BR_ID 6
// #define BR_CAN can1

// //PID param
// #define Chassis_KP 15
// #define Chassis_KI 0.5f
// #define Chassis_KD 0

// #define Gimbal_Follow_KP 10
// #define Gimbal_Follow_KI 0
// #define Gimbal_Follow_KD 0

// #define Power_Control_KP 1
// #define Power_Control_KI 0
// #define Power_Control_KD 0
// #define Power_Control_MAXOUT 120

// //Power Control
// #define Spin_Trans_Ratio 1 //Spin in rad/s, translation in m/s



// //Robo_Gimbal.c
// //Motor Electric param
// #define Pitch_ID 6
// #define Pitch_CAN can2

// #define Yaw_ID 7
// #define Yaw_CAN can2

// //PID param
// #define Pitch_Speed_KP 800
// #define Pitch_Speed_KI 1000
// #define Pitch_Speed_KD 0.7f

// #define Pitch_Position_KP 120
// #define Pitch_Position_KI 0
// #define Pitch_Position_KD 4
// #define Pitch_Position_MAXOUT 360

// #define Pitch_AA_KP 1
// #define Pitch_AA_KI 0
// #define Pitch_AA_KD 0
// #define Pitch_AA_MAXOUT 1


// #define Yaw_Speed_KP 1000
// #define Yaw_Speed_KI 1000
// #define Yaw_Speed_KD 0

// #define Yaw_Position_KP 180
// #define Yaw_Position_KI 0
// #define Yaw_Position_KD 3
// #define Yaw_Position_MAXOUT 1080

// #define Yaw_AA_KP 1
// #define Yaw_AA_KI 0
// #define Yaw_AA_KD 0
// #define Yaw_AA_MAXOUT 1



// //Robo_Shoot.c
// //Motor Electric param
// #define FRIC_L_ID 3
// #define FRIC_L_CAN can2

// #define FRIC_R_ID 1
// #define FRIC_R_CAN can2

// #define DIAL_ID 2
// #define DIAL_CAN can2

// //PID param
// #define FRIC_KP 30
// #define FRIC_KI 600
// #define FRIC_KD 0

// #define DIAL_Speed_KP 5
// #define DIAL_Speed_KI 0
// #define DIAL_Speed_KD 0

// #define DIAL_Position_KP 1
// #define DIAL_Position_KI 0
// #define DIAL_Position_KD 0
// #define DIAL_Position_MAXOUT 360

// //Fire power
// #define Bullet_Speed 700
// #define Fire_Rate 1



// //Robo_AA.c
// #define GRAVITY 9.78
// #define VELOCITY 29.6f
// #define K 0.092f	//or K1 0.038f
// #define BIAS_TIME 100	//In ms, 由于预瞄无法做到完全精准，输入偏置时间
// #define S_BIAS 0.0087	//枪口距离车身中心的距离
// #define Z_BIAS 0.0	//yaw轴电机到枪口水平面的垂直距离

// #define HEADER 0x5A
// #define DETECT_COLOR 0	// 0-red 1-blue




// //For infantry 2
// #else

//Robo_Control.c
#define Gimbal_Follow_Offset -0.6f
#define MAX_Pitch 0.4f
#define MIN_Pitch -0.3f



//Robo_Chassis.c
//Chassis param
#define Wheel_Base 417  //in mm
#define Wheel_Track 371
#define Wheel_Radius 77

#define Motor_Gear_Ratio 19

#define CW 1
#define CCW -1
#define Rotation_Direction CW

//Motor Electric param
#define FL_ID 3
#define FL_CAN can1

#define FR_ID 2
#define FR_CAN can1

#define BL_ID 4
#define BL_CAN can1

#define BR_ID 6
#define BR_CAN can1

//PID param
#define Chassis_KP 15
#define Chassis_KI 0.5f
#define Chassis_KD 0

#define Gimbal_Follow_KP 15
#define Gimbal_Follow_KI 0
#define Gimbal_Follow_KD 0

#define Power_Control_KP 5.0f
#define Power_Control_KI 0.0f
#define Power_Control_KD 0.0f
#define Power_Control_MAXOUT 120

//Power Control
#define Spin_Trans_Ratio 1 //Spin in rad/s, translation in m/s



//Robo_Gimbal.c
//Motor Electric param
#define Pitch_ID 1
#define Pitch_CAN can2

#define Yaw_ID 3
#define Yaw_CAN can1

//PID param
#define Pitch_Speed_KP 800
#define Pitch_Speed_KI 1000
#define Pitch_Speed_KD 0.7f

#define Pitch_Position_KP 120
#define Pitch_Position_KI 0
#define Pitch_Position_KD 4
#define Pitch_Position_MAXOUT 360

#define Pitch_AA_KP 1
#define Pitch_AA_KI 0
#define Pitch_AA_KD 0
#define Pitch_AA_MAXOUT 1


#define Yaw_Speed_KP 1000
#define Yaw_Speed_KI 1000
#define Yaw_Speed_KD 0

#define Yaw_Position_KP 180
#define Yaw_Position_KI 0
#define Yaw_Position_KD 3
#define Yaw_Position_MAXOUT 1080

#define Yaw_AA_KP 1
#define Yaw_AA_KI 0
#define Yaw_AA_KD 0
#define Yaw_AA_MAXOUT 1



//Robo_Shoot.c
//Motor Electric param
#define FRIC_L_ID 2
#define FRIC_L_CAN can2

#define FRIC_R_ID 1
#define FRIC_R_CAN can2

#define DIAL_ID 3
#define DIAL_CAN can2

//PID param
#define FRIC_KP 30
#define FRIC_KI 500
#define FRIC_KD 0.01f

#define DIAL_Speed_KP 10
#define DIAL_Speed_KI 0
#define DIAL_Speed_KD 0

#define DIAL_Position_KP 1
#define DIAL_Position_KI 0
#define DIAL_Position_KD 0
#define DIAL_Position_MAXOUT 360

//Fire power
#define Bullet_Speed 600
#define Fire_Rate 1



//Robo_AA.c
#define GRAVITY 9.78
#define VELOCITY 29.6f
#define K 0.092f	//or K1 0.038f
#define BIAS_TIME 100	//In ms, 由于预瞄无法做到完全精准，输入偏置时间
#define S_BIAS 0.0087	//枪口距离车身中心的距离
#define Z_BIAS 0.0	//yaw轴电机到枪口水平面的垂直距离

#define HEADER 0x5A
#define DETECT_COLOR 0	// 0-red 1-blue

// #endif

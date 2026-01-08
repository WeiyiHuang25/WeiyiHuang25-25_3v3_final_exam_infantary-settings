#ifndef _REFERENCE_H_
#define _REFERENCE_H_
//#include "main.h"
#include "uart_if.h"
#include "timer.h"
#include <stdlib.h>

#define MAXSIZE 100 // 最大长度
#define JUDGE_LOST_TIME ((uint32_t)1000)

#define BLUE 0
#define RED 1

/*------------------串口协议详细说明-----------------------------
 */
/**
 * @brief  比赛状态数据：0x0001。发送频率：3Hz
 */
typedef __packed struct
{
	uint8_t game_type : 4;			// 0-3 bit：比赛类型 //2023年07月07日测试通过
	uint8_t game_progress : 4;	// 4-7 bit：当前比赛阶段
	uint16_t stage_remain_time; // 当前阶段剩余时间，单位 s
	uint64_t SyncTimeStamp;
} ext_game_status_t;

/**
 * @brief 比赛结果数据：0x0002。发送频率：比赛结束后发送
 */
typedef __packed struct
{
	uint8_t winner;
} ext_game_result_t;

/**
 * @brief  机器人血量数据：0x0003。发送频率：3Hz
 */
typedef __packed struct
{
	uint16_t red_1_robot_HP;
	uint16_t red_2_robot_HP;
	uint16_t red_3_robot_HP;
	uint16_t red_4_robot_HP;
	uint16_t red_5_robot_HP;
	uint16_t red_7_robot_HP;
	uint16_t red_outpost_HP;
	uint16_t red_base_HP;
	uint16_t blue_1_robot_HP;
	uint16_t blue_2_robot_HP;
	uint16_t blue_3_robot_HP;
	uint16_t blue_4_robot_HP;
	uint16_t blue_5_robot_HP;
	uint16_t blue_7_robot_HP;
	uint16_t blue_outpost_HP;
	uint16_t blue_base_HP;
} ext_game_robot_HP_t;

/**
 * @brief 场地事件数据：0x0101。发送频率：3Hz
 */
typedef __packed struct
{
	uint32_t event_data;
} ext_event_data_t;

/**
 * @brief 补给站动作标识：0x0102。发送频率：动作改变后发送
 */
typedef __packed struct
{
	uint8_t reserved;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

/**
 * @brief 裁判警告信息：cmd_id (0x0104)。发送频率：己方警告发生后发送
 */
typedef __packed struct
{
	uint8_t level;
	uint8_t offending_robot_id;
	uint8_t count;
} ext_referee_warning_t;

/**
 * @brief 飞镖发射口倒计时：cmd_id (0x0105)。发送频率：3Hz 周期发送
 */
typedef __packed struct
{
	uint8_t dart_remaining_time;
	uint16_t dart_info;
} ext_dart_info_t;

/**
 * @brief  比赛机器人状态：0x0201。发送频率：10Hz
 */
typedef __packed struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t current_HP; 
	uint16_t maximum_HP;
	uint16_t shooter_barrel_cooling_value;
	uint16_t shooter_barrel_heat_limit;
	uint16_t chassis_power_limit;
	uint8_t power_management_gimbal_output : 1;
	uint8_t power_management_chassis_output : 1; 
	uint8_t power_management_shooter_output : 1; 
} ext_robot_status_t;
/**
 * @brief  实时功率热量数据：0x0202。发送频率：50Hz
 */
typedef __packed struct
{
	uint16_t chassis_volt;									// 底盘输出电压
	uint16_t chassis_current;								// 底盘输出电流
	float chassis_power;										// 底盘输出功率
	uint16_t chassis_power_buffer;					// 剩余缓冲能量
	uint16_t shooter_id1_17mm_cooling_heat; // 17mm枪口热量
	uint16_t shooter_id2_17mm_cooling_heat; // 机动17枪口热量
	uint16_t shooter_id1_42mm_cooling_heat; // 42mm枪口热量
} ext_power_heat_data_t;
/**
 * @brief  机器人位置：0x0203。发送频率：10Hz
 */
typedef __packed struct
{
	float x;	 // 位置 x 坐标，单位 m
	float y;	 // 位置 y 坐标，单位 m
	float angle; //测速模块的朝向
} ext_robot_pos_t;
/**
 * @brief  机器人增益：0x0204。发送频率：1Hz
 */
typedef __packed struct
{
	uint8_t recovery_buff;      //机器人回血增益
	uint8_t cooling_buff;		//机器人枪口冷却倍率
	uint8_t defence_buff;		//机器人防御增益
	uint8_t vulnerability_buff; //机器人负防御增益
	uint16_t attack_buff;		//机器人攻击增益
} ext_buff_t;
/**
 * @brief  空中机器人能量状态：0x0205。发送频率：10Hz
 */
typedef __packed struct
{
	uint8_t airforce_status;
	uint8_t time_remain;
} ext_air_support_data_t;
/**
 * @brief  伤害状态：0x0206。发送频率：伤害发生后发送
 */
typedef __packed struct
{
	uint8_t armor_id : 4;
	uint8_t HP_deduction_reason : 4;
} ext_hurt_data_t;
/**
 * @brief  实时射击信息：0x0207。发送频率：射击后发送
 */
typedef __packed struct
{
	uint8_t bullet_type; // 子弹类型: 1：17mm 弹丸 2：42mm 弹丸
	uint8_t shooter_id;	 // 发射机构 ID：1：1 号 17mm 发射机构,2：2 号 17mm 发射机构,3：42mm 发射机构
	uint8_t bullet_freq; // 子弹射频 单位 Hz
	float bullet_speed;	 // 子弹射速 单位 m/s
} ext_shoot_data_t;

/**
 * @brief 子弹剩余发射数：0x0208。10Hz 周期发送，所有机器人发送
 */
typedef __packed struct
{
	uint16_t projectile_allowance_17mm; // 17mm 弹丸允许发弹量
	uint16_t projectile_allowance_42mm; // 42mm 弹丸允许发弹量
	uint16_t remaining_gold_coin;				// 剩余金币数量
} ext_bullet_remaining_t;
/**
 * @brief 机器人 RFID 状态：0x0209。发送频率：3Hz，发送范围：单一机器人
 */
typedef __packed struct
{
	// bit 0：基地增益点 RFID 状态；
	// bit 1：高地增益点 RFID 状态；
	// bit 2：能量机关激活点 RFID 状态；
	// bit 3：飞坡增益点 RFID 状态；
	// bit 4：前哨岗增益点 RFID 状态；
	// bit 6：补血点增益点 RFID 状态；
	// bit 7：工程机器人复活卡 RFID 状态；
	// bit 8-31：保留
	uint32_t rfid_status;
} ext_rfid_status_t;

/**
 * @brief  飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人
 */
typedef __packed struct
{
	uint8_t dart_launch_opening_status; // 当前飞镖发射口的状态
	uint8_t reserved;				// 保留位
	uint16_t target_change_time;				// 切换打击目标时的比赛剩余时间，单位秒，从未切换默认为 0
	uint16_t operate_launch_cmd_time;		// 最近一次操作手确定发射指令时的比赛剩余时间，单位秒, 初始值为 0。
} ext_dart_client_cmd_t;
/**
 * @brief  机器人间交互数据 0x0301
 */
typedef __packed struct
{
	uint16_t data_cmd_id; // 数据段ID
	uint16_t sender_ID;		// 发送者ID
	uint16_t receiver_ID; // 接收者ID
	uint8_t data[20];			// 内容 ID:0x0200~0x02FF
} ext_robot_interactive_data_t;

/**
 * @brief 自定义控制器 0x0302
 */
typedef __packed struct
{
	uint8_t data[30];
} custom_robot_data_t;

/**
 * @brief  客户端小地图交互数据：0x0303。发送频率：触发时发送
 */
typedef __packed struct
{
	float target_position_x;  //目标位置 x 轴坐标，单位 m
	float target_position_y;  //目标位置 y 轴坐标，单位 m
	uint8_t cmd_keyboard;	  //云台手按下的键盘按键通用键值
	uint8_t target_robot_id;  //对方机器人 ID
	uint8_t cmd_source;		  //信息来源 ID
} ext_map_command_t;

/**
 * @brief  图传遥控信息标识：0x0304。发送频率：30Hz。
 */
typedef __packed struct
{
	int16_t mouse_x;					// 鼠标 X 轴信息
	int16_t mouse_y;					// 鼠标 Y 轴信息
	int16_t mouse_z;					// 鼠标滚轮信息
	int8_t left_button_down;	// 鼠标左键
	int8_t right_button_down; // 鼠标右键按下
	__packed union
	{
		__packed struct
		{
			uint16_t W : 1;
			uint16_t S : 1;
			uint16_t A : 1;
			uint16_t D : 1;
			uint16_t SHIFT : 1;
			uint16_t CTRL : 1;
			uint16_t Q : 1;
			uint16_t E : 1;
			uint16_t R : 1;
			uint16_t F : 1;
			uint16_t G : 1;
			uint16_t Z : 1;
			uint16_t X : 1;
			uint16_t C : 1;
			uint16_t V : 1;
			uint16_t B : 1;
		} key;
		uint16_t value;
	} keyboard;
	uint16_t reserved;
} ext_remote_control_t;

/**
	* @brief  小地图接收信息标识：0x0305。最大接收频率：10Hz。
雷达站发送的坐标信息可以被所有己方操作手在第一视角小地图看到。
	*/
typedef __packed struct
{
	uint16_t target_robot_ID; // 目标机器人 ID
	float target_position_x;	// 目标 x 位置坐标，单位 m
	float target_position_y;	// 目标 y 位置坐标，单位 m
} ext_map_robot_data_t;

/**
	* @brief  小地图接收哨兵上报路径：0x0307
	哨兵机器人可向己方空中机器人选手端发送路径坐标数据，该路径会在其小地图上显示
	@attention:
	小地图左下角为坐标原点，水平向右为 X轴正方向，竖直向上为 Y 轴正方向。
	显示位置将按照场地尺寸与小地图尺寸等比缩放，超出边界的位置将在边界处显示
	*/
typedef __packed struct
{
	uint8_t intention;				 // 1：到目标点攻击;2：到目标点防守;3：移动到目标点
	uint16_t start_position_x; // 路径起点 X 轴坐标，单位：dm
	uint16_t start_position_y; // 路径起点 Y 轴坐标，单位：dm
	int8_t delta_x[49];				 // 路径点 X 轴增量数组，单位：dm
	int8_t delta_y[49];				 // 路径点 Y 轴增量数组，单位：dm
	uint16_t sender_id;
} map_data_t;

/**
	* @brief  向选手端发送UI信息：0x0308
	己方机器人可通过常规链路向己方任意选手端发送自定义的消息，该消息会在己方选手端特定位置显示。
	*/
typedef __packed struct
{ 
	uint16_t sender_id;
	uint16_t receiver_id;
	uint8_t user_data[30];
} custom_info_t;

/**
 * @brief 串口协议对应命令ID
 */
typedef enum
{
	cmd_game_status = 0x0001,											/**< 比赛状态id */
	cmd_game_result = 0x0002,											/**< 比赛结果id */
	cmd_game_robot_HP = 0x0003,										/**< 机器人生命id */
	cmd_event_data = 0x0101,											/**< 场地事件数据id */
	cmd_supply_projectile_action = 0x0102,				/**< 补给站动作标识 */
	cmd_referee_warning = 0x0104,									/**< 裁判警告信息 */
	cmd_dart_remaining_time = 0x0105,							/**< 飞镖发射口倒计时 */
	cmd_game_robot_status = 0x0201,								/**< 机器人状态id */
	cmd_power_heat_data = 0x0202,									/**< 实时功率热量id */
	cmd_game_robot_pos = 0x0203,									/**< 机器人位置id */
	cmd_buff = 0x0204,														/**< 机器人增益id */
	cmd_aerial_robot_energy = 0x0205,							/**< 空中机器人能量状态id */
	cmd_robot_hurt = 0x0206,											/**< 伤害状态id */
	cmd_shoot_data = 0x0207,											/**< 实时射击信息id */
	cmd_bullet_remaining = 0x0208,								/**< 子弹剩余发射数id */
	cmd_rfid_status = 0x0209,											/**< 机器人 RFID 状态id */
	cmd_dart_client_cmd = 0x020A,									/**< 飞镖机器人客户端指令id */
	cmd_student_interactive_header_data = 0x0301, /**< 机器人间通信id */
	cmd_interactive_data = 0x302,									/**< 自定义控制器id */
	cmd_ext_picture_command = 0x0303,							/**<客户端小地图交互数据id */
	cmd_robot_command = 0x0304,										/**< 图传遥控信息id */
	cmd_client_map_command = 0x0305,							/**<小地图接收雷达信息id */
	cmd_custom_info = 0x0308									/**<发送UI信息id */
} CmdID;
/**
 * @brief 裁判系统数据包长度
 */
#define LEN_game_status 11
#define LEM_game_result 1
#define LEN_game_robot_HP 32
#define LEN_event_data 4
#define LEN_supply_projectile_action 4
#define LEN_referee_warning 3
#define LEN_dart_remaining_time 3
#define LEN_game_robot_state 13
#define LEN_power_heat_data 16
#define LEN_game_robot_pos 12
#define LEN_buff_musk 6
#define LEN_aerial_robot_energy 2
#define LEN_robot_hurt 1
#define LEN_shoot_data 7
#define LEN_bullet_remaining 6
#define LEN_rfid_status 4
#define LEN_dart_client_cmd 6
#define LEN_student_interactive_header_data 26
#define LEN_robot_interactive_data 30
#define LEN_picture_command 12
#define LEN_robot_command 12
#define LEN_client_map_command 10
#define LEN_custom_info 34

/**
 * @brief 裁判系统句柄
 */
typedef struct
{
	struct
	{
		UART_Handle_t uart;					/**< 裁判系统挂载的串口句柄 */
		uint8_t rx_buffer[MAXSIZE]; /**< 接收缓冲句柄 */
		struct
		{
			SEML_StatusTypeDef data_validity : 2; /**< 数据有效性 */
		} status;
	} config;																															 /**< 裁判系统配置 */
	ext_game_status_t game_status;																				 /**< 游戏状态 */
	ext_game_robot_HP_t game_robot_HP;																		 /**< 机器人血量 */
	ext_event_data_t event_data;																					 /**< 场地事件数据 */
	ext_supply_projectile_action_t supply_projectile_action;							 /**< 补给站动作标识 */
	ext_referee_warning_t referee_warning;																 /**< 裁判警告信息 */
	ext_dart_info_t dart_remaining_time;												 /**< 飞镖发射口倒计时 */
	ext_robot_status_t game_robot_status;														 /**< 机器人当前规则参数 */
	ext_power_heat_data_t power_heat_data;																 /**< 功率热量数据 */
	ext_robot_pos_t game_robot_pos;																	 /**< 机器人位置数据 */
	ext_buff_t buff;																											 /**< 机器人增益 */
	ext_air_support_data_t aerial_robot_energy;														 /**< 空中机器人能量状态 */
	ext_hurt_data_t robot_hurt;																					 /**< 机器人伤害状态 */
	ext_shoot_data_t shoot_data;																					 /**< 实时射击信息 */
	ext_bullet_remaining_t bullet_remaining;															 /**< 子弹剩余发射数 */
	ext_rfid_status_t rfid_status;																				 /**< 机器人 RFID 状态 */
	ext_dart_client_cmd_t dart_client_cmd;																 /**< 飞镖机器人客户端指令数据 */
	ext_robot_interactive_data_t student_interactive_header_data; /**< 机器人间交互数据 */
	ext_map_command_t robot_command;																		 /**< 客户端小地图交互数据 */
	ext_remote_control_t picture_command;																 /**< 图传遥控信息标识 */
	ext_map_robot_data_t client_map_command;													 /**< 小地图接收信息标识 */
	custom_info_t ui_command;
#ifdef USE_SOFT_WITCHDOG
	Soft_WitchDog_t witchdog; /**< 软件看门狗*/
#endif
} Reference_t;

/**
 * @brief 裁判系统初始化
 * @param[out] handle 裁判系统句柄
 * @param[in] huart 裁判系统挂载的串口句柄
 */
void Reference_Init(Reference_t *handle, UART_IF_t *huart);
/**
 * @brief 裁判系统接收回调函数
 * @param[in] uart 串口句柄
 */
void Reference_Rx_Callback(UART_Handle_t *uart);

/**
 * @brief 裁判系统超时回调函数
 * @param[in] handle 串口句柄
 */
void Reference_Timeout_Callback(Reference_t *handle);

/**
 * @brief  解析接收到的裁判系统信息
 * @param[in,out] handle 裁判系统句柄
 */
SEML_StatusTypeDef Reference_Rx_Handle(Reference_t *handle);

/**
 * @brief 裁判系统失联检测
 * @return 1:裁判系统失联,0:裁判系统未失联
 */
#define Check_Reference_Lost(handle) ((handle)->config.status.data_validity == SEML_TIMEOUT)

#endif

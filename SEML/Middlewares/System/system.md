# 系统库

## 消息驱动和事件驱动

消息驱动和事件驱动是一种编程思想，其目的是为了**解耦各模块之间的耦合**。两者都是基于发布（publisher）-订阅（subscriber）机制。

### 消息驱动

消息驱动(Message-Driven)是基于发布（publisher）-订阅（subscriber）机制

\- 发布者：发布消息的对象。发布者会将自己的消息推送给所有订阅了某个特定**话题**的订阅者。

\- 订阅者：获取消息的对象。订阅者在订阅了某个话题之后，可以通过接口获得该话题的消息。

\- 话题（topic）：用于区分消息来源的对象。可以将一个话题看作一刊杂志，不同的发布者会将文章汇集到杂志上，而订阅者选择订阅一种杂志，然后就可以获取所有写在杂志上的文章。在消息列表模块中，使用一串数字ID来表示。

\- 消息中心：处理消息的中间商，可以理解为邮差。在SEML库中，消息中心为消息列表。

​	消息中心作为不同应用间进行消息传递的中介，它的存在可以在相当大的程度上解耦不同的app，使得不同的应用之间**不存在包含关系**，让代码的自由度更大，将不同模块之间的关系降为**松耦合**。在以往，如果一个.c文件中的数据需要被其他任务/源文件共享，那么其他模块应该要包含前者的头文件，且头文件中应当存在获取该模块数据的接口（即函数，一般是返回数据指针或直接返回数据，**强烈建议不要使用全局变量**）；但现在，不同的应用之间完全隔离，他们不需要了解彼此的存在，而是只能看见一个**消息中心**以及一些**话题**。

​	需要被共享的消息，将会被**发布者**（publisher）发送到消息中心；要获取消息，则由**订阅者**（subscriber）从消息中心根据订阅的话题获取。在这之前，发布者要在消息中心完成**注册**，将自己要发布的消息类型和话题名称提交到消息中心；订阅者同样可以消息中心完成**订阅**，将自己要接收的消息类型和话题名称提交到订阅中心。消息中心会根据**话题名称**，把订阅者绑定到发布相同名称的发布者上。

​	消息列表只对最新数据进行缓冲，若需要**无信息损失**(无数据损失、无顺序损失)地进行读取需要向对应话题注册一个队列使其形成消息队列(message Queue)。

### 事件驱动

​	事件驱动跟消息驱动很像，但是主要区别是消息驱动是**发布者将消息发布到消息中心，消息中心再发送给接收者**，通过阻塞进行线程切换或者轮询操作进行异步操作，属于阻塞式。而事件驱动是**发布者发布给消息中心，消息中心使用回调函数来调用接收者**从而进行异步操作，属于协作式。

​	然而，事件驱动机制也有一些缺点。首先，**它很难实现复杂的业务逻辑，因为回调函数的执行顺序是不可控的**。另外，如果回调函数内部出现错误，那么很难对错误进行调试。消息驱动弥补事件驱动的劣势。

​	总的来说，事件驱动机制和消息驱动机制各有优缺点，适用场景也不同。如果需要实现**简单的**、**异步的**任务处理，事件驱动机制是一个很好的选择；如果需要实现**复杂的**、**需要多个对象协同工作的任务处理**，消息驱动机制是一个更好的选择。

​	基于此，SEML库的消息列表采用基于消息的事件驱动形式进行通讯，它能够同时支持消息驱动和事件驱动两种编程机制。可以按照需要进行使用。

## 消息列表模块

### 简介

​	消息列表是为了解决线程间通讯的问题而设计的，使用消息订阅与发布机制，支持多个一对多、多对多、多对一的形式，采用**基于消息的事件驱动**从而进行通讯的消息中心。SEML库中的CAN接口层便是基于其设计的。

​	消息列表以**ID的形式作为话题(topic)**来识别每一个报文，支持以**控制码(4字节)**，**数据(不定长)**的形式发送至消息列表，需要该话题的可以**注册事件回调**或者**直接用ID来读取消息包**。

​	消息列表支持注册常驻消息和临时消息。

- ​	常驻消息为经常使用的消息或者事件，需要调用`message_List_Register`进行注册，它不会被消息中心回收。

- ​	临时消息为临时产生的消息，不需要注册，在一段时间未被使用后会被消息中心回收。

​	消息列表发送使用`message_List_Update_Cmd`和`message_List_Update_Data`分别发送控制码和数据，同时也支持使用`message_List_Update`同时发送控制码和数据。

​	事件可以在回调函数中采用`Get_message_Pack_Cmd`,`Get_message_Pack_Data`来获取控制码和数据。

​	消息可以采用`Get_message_ID_Cmd`,`Get_message_ID_Data`根据消息id来获取控制码和数据。

### 如何使用

常用的函数定义如下：

```c
/**
 * @brief 初始化消息队列
 * @param[out] message_ID_list 消息列表句柄
 * @param[in] id_buffer id缓存数组
 * @param[in] size 缓存大小
 * @param[in] timeout 超时时间
 */
void message_List_Init(message_List_t *message_list, message_Pack_t *pack_buffer, uint16_t size, uint32_t timeout);
/**
 * @brief 在消息列表中进行注册
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] fun 消息回调函数指针
 * @param[in] config 消息回调函数句柄
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 重复注册
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef message_List_Register(message_List_t *message_list, uint16_t ID, void (*fun)(void *config, message_Pack_t const *const message_pack), void *config);
/**
 * @brief 更新消息包控制码
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] cmd 控制码
 */
#define message_List_Update_Cmd(message_list,id,cmd) message_List_Update(message_list,id,cmd,NULL,0)
/**
 * @brief 更新消息包数据
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] data 数据指针
 * @param[in] data_longth 数据长度
 */
#define message_List_Update_Data(message_list,id,data,longth) message_List_Update(message_list,id,0x00,data,longth)
/**
 * @brief 获取消息包控制码
 * @param[in] message_pack 消息包句柄
 * @return 该消息包控制码
 */
uint16_t Get_message_Pack_Cmd(message_Pack_t const *const message_pack);
/**
 * @brief 获取消息包数据包
 * @param[in] message_pack 消息包句柄
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_message_Pack_Data(message_Pack_t const *const message_pack, void **data);
/**
 * @brief 获取特定id的消息包更新时间戳
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包更新时间戳
 */
uint32_t Get_message_ID_Timestamp(message_List_t *message_list, uint16_t id);
/**
 * @brief 获取特定id的消息包控制码
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包控制码
 */
uint16_t Get_message_ID_Cmd(message_List_t *message_list, uint16_t id);
/**
 * @brief 获取特定id的消息包数据包
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_message_ID_Data(message_List_t *message_list, uint16_t id, void **data);

/**
 * @brief 获取消息包数据包
 * @param[in] message_pack 消息包句柄
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_message_Pack_Data(message_Pack_t const *const message_pack, void **data);

/**
 * @brief 获取特定id的消息包更新时间戳
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包更新时间戳
 */
uint32_t Get_message_ID_Timestamp(message_List_t *message_list, uint16_t id);

/**
 * @brief 获取特定id的消息包控制码
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包控制码
 */
Cmd_t Get_message_ID_Cmd(message_List_t *message_list, uint16_t id);

/**
 * @brief 获取特定id的消息包数据包
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_message_ID_Data(message_List_t *message_list, uint16_t id, void **data);
```

消息列表采用静态初始化的方式，需要传入消息列表句柄和定义后的消息缓存进行初始化。

对于需要使用的id需要进行注册，通过传入回调函数可以注册事件回调

需注意的是，如果是临时变量可以不注册，经过一段时间没被使用后会被清理。

demo如下：

```c
// 该demo演示了接收到imu板发送过来的id为0x022的数据进行解算后发送到id为0xff22,交给后续处理
// 通常can和线程通讯使用的并不是同一个消息列表，但是此处为了演示方便就使用同一个消息列表
// 为了避免爆栈，声明大数据请放在全局变量
message_Pack_t message_buffer[15];
message_List_t message_list;
int main()
{
    acc_raw_data_t *buffer；
    message_Pack_t *pack;
    ...
    // 消息列表初始化 定义1000ms后超时
    message_List_Init(&message_list, message_buffer, 15, 1000);
    // 注册id，注册事件回调
    message_List_Register(&message_list, 0x022, acc_callback, NULL);
    // 注册id，注册为消息
    message_List_Register(&message_list, 0x0ff22, NULL, NULL);
    // 注册canj接收回调函数
    HAL_CAN_RegisterCallback(&hcan1, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, SEML_CAN_RxCallback);
    while(1)
    {
        // 获取id为0xff22的数据
        Get_message_ID_Data(&message_list,0xff22,&buffer);
        ...
    }
}
void acc_callback(void *config, message_Pack_t const *const message_pack)
{
	static uint16_t longth;
	uint8_t *buffer;
    acc_raw_data_t rx_accel;
    // 获取消息id为0x022的数据
	longth = Get_message_Pack_Data(message_pack,&buffer);
    // 对其进行解算
	rx_accel.x = (int16_t)((buffer[0] << 8) | buffer[1]) / 100.0f;
	rx_accel.y = (int16_t)((buffer[2] << 8) | buffer[3]) / 100.0f;
	rx_accel.z = (int16_t)((buffer[4] << 8) | buffer[5]) / 100.0f;
	// 更新消息id为0xff22消息包，发送解算后数据
    message_List_Update_Data(&message_list, 0xff22, rx_accel, sizeof(rx_accel));
}
// can接收回调
void SEML_CAN_RxCallback(void *hcan)
{
 	uint8_t data[8];
    // 从can接收邮箱中读取数据
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMessage,data);
    // 用can的id更新消息消息包
	message_List_Update(&message_list,RxMessage.StdId,RxMessage.RTR,data,RxMessage.DLC);
}
```

## 软件定时器模块

软件定时器是通过软件方法，将一个硬件定时器进行扩充，可无限扩展你所需的定时器任务，取代传统的标志位判断方式， 更优雅更便捷地管理程序的时间触发时序。

常用的函数原型如下：

```c
/**
 * @brief 软件定时器句柄
 */
typedef struct
{
	void (*callback_fun)(void *config); /**< 回调函数 */
	void *callback_config;							/**< 回调函数配置句柄 */
	uint32_t timeout_count;							/**< 定时器计数器 */
	uint32_t timeout_buckup;						/**< 定时器重装载值 */
	Timer_AutoReload mode : 1;					/**< 定时器运行模式 */
	Timer_Status status : 1;						/**< 定时器运行状态 */
	void *next;													/**< 下一个定时器 */
} Soft_Timer_t;
/**
 * @brief 软件定时器初始化
 * @param[out] hstim 软件定时器句柄
 * @param[in] mode 定时器模式
 * @param[in] timeout 定时时间(以调用的硬件定时器为时基的整数倍)
 * @param[in] callback_fun 回调函数
 * @param[out] callback_config 回调函数配置
 */
void SEML_Timer_Create(Soft_Timer_t *hstim, Timer_AutoReload mode, uint32_t timeout, void (*callback_fun)(void *config), void *callback_config);
/**
 * @brief 删除软件定时器
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Delete(Soft_Timer_t *hstim);
/**
 * @brief 软件定时器复位
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Reset(Soft_Timer_t *hstim);
/**
 * @brief 软件定时器启动
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Start(Soft_Timer_t *hstim);
/**
 * @brief 软件定时器关闭
 * @param[out] hstim 软件定时器句柄
 */
void SEML_Timer_Stop(Soft_Timer_t *hstim);
/**
 * @brief 软件定时器回调函数
 * @param[in] config 回调函数配置
 */
void SEML_Timer_Callback(void *config);
```

### 初始化

软件定时器需要一个硬件定时器作为时基，将其注册为该硬件定时器的回调函数，并且开启该定时器：

```c
// htim6配置为一个1ms定时器，软件定时器将以1ms作为时基
HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, SEML_Timer_Callback);
HAL_TIM_Base_Start_IT(&htim6);
```

### 创建一个软件定时器

软件定时器创建后默认开启，**以硬件定时器为时基**，定时值为硬件定时器频率的整数倍。

```c
Soft_Timer_t ahrs_timer, init_timer, uart_timer, led_timer;
// 创建一个循环执行的200ms的定时器
SEML_Timer_Create(&init_timer, Timer_Cycle, 200, init_display, NULL);
// 创建一个循环执行的1ms定时器，传入参数为ahrs句柄
SEML_Timer_Create(&ahrs_timer, Timer_Cycle, 1, AHRS_Update, &ahrs);
```

### 删除一个软件定时器

```c
// 删除一个软件定时器
SEML_Timer_Delete(&init_timer);
```

## 软件看门狗模块

软件看门狗是基于软件定时器的一个拓展模块，需要先初始化软件定时器。

看门狗是检测程序运行状态的一个软件模块，如果超过一定时间没喂狗则会执行该软件的复位回调函数。

常用的函数原型如下：

```c
typedef Soft_Timer_t Soft_WitchDog_t;

/**
 * @brief 软件看门狗注册
 * @note 需要在句柄中加入看门狗句柄定义(Soft_WitchDog_t witchdog)
 * @param[out] handle 要注册看门狗的句柄
 * @param[in] timeout 超时时间
 * @param[in] callback_fun 超时回调函数
 */
#define SEML_WitchDog_Register(handle,timeout,callback_fun) SEML_Timer_Create(&(handle)->witchdog,Timer_Cycle,timeout,callback_fun,handle)

/**
 * @brief 喂狗
 * @param[out] handle 句柄
 */
#define SEML_Feed_WitchDog(handle) SEML_Timer_Reset(&(handle)->witchdog)

```

## 事件操作系统模块

事件操作系统模块是SEML库内置的一个小型操作系统，该操作系统为协作式内核(任务不释放cpu不会执行下一个任务)，仅允许高优先级任务抢占低优先级任务。

事件操作系统使用优先级功能需要占用一个定时器(软件硬件皆可)。

常用的函数原型如下：

```c
/**
 * @brief 注册任务
 * @param[out] task_handle 任务句柄
 * @param[in] loop_delay 循环延时,该传入0不使能定时器
 * @param[in] priority 任务优先级
 * @param[in] Constructor 任务构造函数
 * @param[in] Event_Task 任务回调函数
 * @param[in] Destructor 任务析构函数
 */
void Event_OS_Task_Register(Event_Task_Handle_t *task_handle, uint32_t loop_delay, Event_OS_Priority_t priority, Task_Constructor_t Constructor, Event_Task_t Event_Task, Task_Destructor_t Destructor);
/**
 * @brief 注销任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Logout(Event_Task_Handle_t *task_handle);
/**
 * @brief 启动任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Start(Event_Task_Handle_t *task_handle);
/**
 * @brief 暂停任务
 * @param[out] task_handle 任务句柄
 */
void Event_OS_Task_Pause(Event_Task_Handle_t *task_handle);
/**
 * @brief 事件操作系统初始化
 */
void Event_OS_Init(void);
/**
 * @brief 启动事件驱动操作系统
 */
void Event_OS_Run(void);
/**
 * @brief 事件操作系统回调函数
 * @param[in] config 回调函数配置
 */
void Event_OS_Callback(void *config);
/**
 * @brief 获取cpu使用率
 * @return cpu使用率(0-100)
*/
uint8_t Evnet_OS_Get_CPU_Utilization(void);
```

### 初始化

和软件定时器一样，需要注册回调函数：

```c
// 初始化事件操作系统
Event_OS_Init();
// 注册事件操作系统回调函数
HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, Event_OS_Callback);
HAL_TIM_Base_Start_IT(&htim7);
```

### 新建任务

事件操作系统任务由构造函数(选填)、任务体(必须)、析构函数(选填)组成。

在开始任务的时候会执行构造函数对任务进行初始化，在注销任务的时候会执行析构函数进行释放。

需要先初始化事件操作系统后进行任务注册，事件操作系统以SEML库的时基为基准，若为hal库，默认为1ms.

```c
// 任务句柄必须声明为静态或者全局
static Event_Task_Handle_t control_task_handle,chassis_task_handle,gambal_task_handle;
// 任务注册：高优先级 1ms执行一次。
Event_OS_Task_Register(&control_task_handle,1,EOS_High_Priority,Robo_Control_Init,Robo_Control_Task,NULL);
Event_OS_Task_Register(&chassis_task_handle,1,EOS_High_Priority,Chassis_Init,Chassis_Task,NULL);
Event_OS_Task_Register(&gambal_task_handle,1,EOS_High_Priority,Gimbal_Init,Gimbal_Task,NULL);
```

### 开始事件操作系统

执行`Event_OS_Run();`即可开始，内置死循环，不会执行之后的内容。


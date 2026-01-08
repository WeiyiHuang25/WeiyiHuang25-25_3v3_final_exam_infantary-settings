# 航姿参考系统库

​		AHRS提供了对陀螺仪、加速度计、磁力计进行解算和管理的一个模块。内置Madgwick算法和Mahony算法，可以获取到imu数据后直接解算。

## 航姿参考系统

### 	介绍

​		航姿参考系统包括基于MEMS的三轴陀螺仪，加速度计和磁强计。简单来说就是IMU（基于惯性原理的惯性传感器）+磁力计。

#### 陀螺仪

​		陀螺仪是测量在三个正交方向上旋转的角速度的传感器。陀螺仪有许多种类，不同的陀螺仪一般基于不同的工作原理，能够达到的精度也不一样，市场上最常用的微机（MEMS）陀螺仪的基本原理是利用旋转时产生的科里奥利力引发电容的变化，从而将旋转的角速度转化为电信号。

​		以下为MEMS陀螺仪的内部构造：

<img src="https://img-blog.csdnimg.cn/cebbe5fdb11f4608a5277adbfd3e702e.png" alt="MEMS式陀螺仪" style="zoom: 33%;" />

​		在 MEMS 陀螺仪的设计上，这个物体需要被驱动，不停地来回做径向运动或者震荡，与此对应的科里奥利力就是不停地在横向来回变化，并使物体在横向作微小震荡。MEMS 陀螺仪通常有两个方向的可移动电容板。径向的电容板加震荡电压迫使物体作径向运动，横向的电容板测量由于横向科里奥利运动带来的电容变化。因为电容变化、科里奥利力以及角速度三者相互关联，所以由电容的变化可以计算出角速度。

#### 加速度计

​		加速度计是测量三个正交方向上的加速度的传感器。MEMS 加速度计原理是利用加速度变化使内部质量块产生的力发生变化，从而改变电容大小，转化为电信号。当物体静止时，加速度计测量重力加速度在三个正交方向上的分量，配合陀螺仪的角速度信息可以解算出物体的空间位姿。

<img src="image\jiasuduji.png" alt="jiasuduji" style="zoom: 67%;" />

#### 磁力计

​		磁力计是测量磁场强度和方向的传感器。它的原理跟指南针原理类似，可以测量出当前设备与东南西北四个方向上的夹角。它在无人机、智能手表、导航设备中广泛普及和应用。

​		总的来说，陀螺仪知道“我们转了个身”，加速计知道“我们又向前走了几米”，而磁力计则知道“我们是向西方向”的。

## 如何使用

使用到的函数原型：

```c
typedef struct
{
	Euler_Data_t euler_angle;								 /**< 欧拉角 */
	Accel_Data_t accel;										 /**< 加速度 */
	Accel_Data_t real_accel;								 /**< 惯性坐标系下的加速度 */
	Gyro_Data_t gyro;										 /**< 角速度 */
	Mag_Data_t mag;											 /**< 磁场 */
	float quat[4];											 /**< 姿态四元数 */
	float g_norm;											 /**< 标定时的重力加速度 */
	float sample_time;										 /**< 采样时间 */
	Gyro_Data_t gyro_offset;								 /**< 角速度零偏校正值 */
	AHRS_Calculate_fun_t AHRS_Calculate_fun;				 /**< AHRS解算算法 */
	void *IMU_handle;										 /**< IMU句柄*/
	Get_IMU_fun_t Get_IMU_fun;								 /**< 获取IMU数据函数 */
	void *mag_handle;										 /**< 磁力计句柄*/
	Get_Mag_fun_t Get_Mag_fun;								 /**< 获取磁力计数据函数 */
	uint8_t count;											 /**< 标定进度 */
} AHRS_t;

/**
 * @brief AHRS初始化
 * @param[out] AHRS AHRS句柄
 * @param[in] sample_time 采样时间
 * @param[in] AHRS_Calculate_fun AHRS解算算法
 * @param[in] Get_IMU_fun 获取IMU数据(加速度、角速度)函数
 * @param[in] IMU_handle IMU句柄
 * @param[in] Get_Mag_fun 获取磁场数据函数
 * @param[in] mag_handle 磁力计句柄
 */
void AHRS_Init(AHRS_t *AHRS, float sample_time, AHRS_Calculate_fun_t AHRS_Calculate_fun, Get_IMU_fun_t Get_IMU_fun, void *IMU_handle, Get_Mag_fun_t Get_Mag_fun, void *mag_handle);

/**
 * @brief 矫正角速度零飘
 * @param[in,out] AHRS AHRS句柄
 * @return 函数执行状态
 *  - SEML_OK 矫正成功
 * 	- SEML_TIMEOUT 矫正超时
 *  - SEML_ERROR 矫正失败(IMU数据异常)
 */
SEML_StatusTypeDef Calibrate_IMU_Offset(AHRS_t *AHRS);

/**
 * @brief 更新AHRS数据
 * @param[in,out] AHRS AHRS句柄
 */
void AHRS_Update(AHRS_t *AHRS);

/**
 * @brief 获取AHRS欧拉角数据
 * @param[in,out] AHRS AHRS句柄
 * @return 欧拉角数据结构体
 */
#define Get_AHRS_Euler(AHRS) (AHRS->euler_angle)
```

### 初始化

使用前需要初始化imu(和磁力计),确保能正常读取后调用`AHRS_Init`进行初始化。若使用磁力计可以传入磁力计相关配置。不使用可不传入。

```c
/**
 * @brief AHRS初始化
 * @param[out] AHRS AHRS句柄
 * @param[in] sample_time 采样时间
 * @param[in] AHRS_Calculate_fun AHRS解算算法
 * @param[in] Get_IMU_fun 获取IMU数据(加速度、角速度)函数
 * @param[in] IMU_handle IMU句柄
 * @param[in] Get_Mag_fun 获取磁场数据函数
 * @param[in] mag_handle 磁力计句柄
 */
void AHRS_Init(AHRS_t *AHRS, float sample_time, AHRS_Calculate_fun_t AHRS_Calculate_fun, Get_IMU_fun_t Get_IMU_fun, void *IMU_handle, Get_Mag_fun_t Get_Mag_fun, void *mag_handle);
```

参数说明如下:

| 参数               | 解释                                                         | 值                                                          |
| ------------------ | ------------------------------------------------------------ | ----------------------------------------------------------- |
| AHRS               | AHRS解算句柄，声明后传入即可                                 |                                                             |
| sample_time        | 采样时间,为AHRS解算的执行频率，该值必须和实际相对应，否则会有严重的解算误差 | 建议为0.001f                                                |
| AHRS_Calculate_fun | 采用的AHRS解算算法("MadgwickAHRS.h"或"Mahony's")             | Mahony_AHRS_Update<br />Madgwick_AHRS_Update                |
| Get_IMU_fun        | 获取imu角速度和加速度的函数，回调函数原型如下：<br />`void (*Get_IMU_fun_t)(void *config, Accel_Data_t *accel, Gyro_Data_t *gyro);` | 若为bmi088 该值为：BMI088_Read                              |
| IMU_handle         | imu句柄                                                      | 为初始化imu时使用的句柄                                     |
| Get_Mag_fun        | 获取磁力计磁强的函数，回调函数原型如下：<br />`void (*Get_Mag_fun_t)(void *config, Mag_Data_t *mag);` | 若为ist8310 该值为:<br />ist8310_read_mag，或者可以传入NULL |
| mag_handle         | 磁力计句柄                                                   | 为初始化磁力计时使用的句柄                                  |

demo:

```c
// 初始化AHRS
BMI088_Data_t bmi088;
ist8310_Handle_t ist8310;
AHRS_t ahrs;
// 初始化BMI088
Hardware_SPI_Register(&bmi088.SPI_Handle, &hspi1);
SEML_GPIO_Pin_Register(&bmi088.CS1_Accel_handle, CS1_Accel_GPIO_Port, CS1_Accel_Pin);
SEML_GPIO_Pin_Register(&bmi088.CS1_Gyro_handle, CS1_Gyro_GPIO_Port, CS1_Gyro_Pin);
while (BMI088_Init(&bmi088));
// 初始化ist8310
Hardware_I2C_Register(&ist8310.I2C_Handle, &hi2c1);
SEML_GPIO_Pin_Register(&ist8310.RSTN_Pin, RSTN_IST8310_GPIO_Port, RSTN_IST8310_Pin);
if (ist8310_init(&ist8310) == 0)
	AHRS_Init(&ahrs, 0.001f, Madgwick_AHRS_Update, BMI088_Read, &bmi088, ist8310_read_mag, &ist8310);
else
	AHRS_Init(&ahrs, 0.001f, Madgwick_AHRS_Update, BMI088_Read, &bmi088, NULL, NULL);
// 0.001秒的采样频率(ahrs更新频率)，使用Madgwick算法
```

### 零飘标定

​		零漂（零点漂移）指的是传感器在输入电压为零的情况下，输出电压偏离零值的情况。

​		由于imu角速度计存在零飘，使用前需要对其进行标定，以获得准确的角速度数据，调用`Calibrate_IMU_Offset(&ahrs);`进行标定，标定期间尽量减少扰动，以获得准确的标定数值,访问`ahrs->count`可以获得标定进度。

### 温漂

​		温漂是陀螺仪在不同温度下，零点漂移的值不同。通常是要使用imu加热电路对其进行加热稳定在一个恒定值。但是相比于零飘，温漂带来的影响相对较小。可以不标定。

### 更新ahrs

​		ahrs需要严格按照初始值定时执行，以便进行解算，推荐使用定时器进行更新（以软件定时器为例）：

```c
Soft_Timer_t ahrs_timer;
SEML_Timer_Create(&ahrs_timer, Timer_Cycle, 1, AHRS_Update, &ahrs);
```


/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : supercap.h
 * @author : SY7_yellow
 * @brief  : 超级电容通讯模块
 * @date   : 2023-9-17
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-04-18     <td>1.0         <td>SY7_yellow  <td>创建初始版本
 * <tr><td>2023-09-17     <td>1.1         <td>SY7_yellow  <td>移植到SEML库中
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * ** 软件定时器 **
 * 软件定时器是基于硬件定时器的基础上进行拓展，模拟出更多时基的定时器的一个模块,
 * 软件定时器运行模式有单次模式(定时时间到后销毁)和循环模式.
 * 定义一个Soft_Timer_t变量,使用SEML_Timer_Create配置好定时时间,运行模式,回调函数
 * 后将SEML_Timer_Callback注册(或放入)硬件定时器中断中.定时器初始化后默认是使能的,
 * 如果不需要使能可以执行SEML_Timer_Stop关闭定时器.
 * ** 软件看门狗 **
 * 软件看门狗是基于软件定时器的一个衍生,所以需要先配置好软件定时器.
 * 在要使用看门狗的句柄中加入下列定义:
 * Soft_WitchDog_t witchdog;
 * 即可使用软件看门狗,在句柄初始化函数中调用SEML_WitchDog_Register
 * 记得喂狗即可.
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef __SUPERCUP_H_
#define __SUPERCUP_H_
#include "SEML_common.h"
#include "can_if.h"
/// @brief 超级电容超时时间
#define SUPERCAP_CONNECT_TIMEOUT 1000

#define SUPERCAP_RX_ID 0x030
#define SUPERCAP_CONFIG_ID 0x02f
#define SUPERCAP_DATA_ID 0x02e

/**
 * @brief 超级电容句柄
 */
typedef struct
{
	struct
	{
		float Vcap;			/**< 电容电压 */
		float Pchassis; /**< 底盘功率 */
		float Pcharge;	/**< 电容充电功率 */
		float Vchassis; /**< 底盘电压 */
	} feedbuck_value; /**< 反馈值 */
	struct
	{
		uint8_t power_limit;							/**< 底盘功率上限 */
		uint8_t power_compensation_limit; /**< 补偿功率上限 */
		uint8_t power_charge_limit;				/**< 充电功率上限 */
		uint8_t cap_enable;								/**< 超级电容开关 */
	} config_value;											/**< 设置值 */
	union
	{
		uint8_t value;
		struct
		{
			uint8_t CapEnable : 1;							/**< 电容使能 */
			uint8_t CapUndervoltage : 1;				/**< 电容欠压 */
			uint8_t PowerCompensationLimit : 1; /**< 补偿达到上限 */
			uint8_t RESERVER : 4;								/**< 保留位 */
			uint8_t PowerLoopError : 1;					/**< 电源环路异常 */
		} flag;
	} status;							 /**< 状态值 */
	uint32_t rx_timestamp; /**< 超级电容接收时间戳 */
	Can_Handle_t *can_handle;
} SuperCap_t;

/**
 * @brief 设置底盘期望功率
 * @param[out] handle 超级电容句柄
 * @param[in] power_limit 功率限制值,超级电容将会将输入功率限制到该功率
 */
#define Set_SuperCap_PowerLimit(handle, _power_limit) (handle->config_value.power_limit = (_power_limit))

/**
 * @brief 设置电容补偿功率
 * @param[out] handle 超级电容句柄
 * @param[in] compensation_limit 功率补偿值,超过限制值部分所能补偿的最大功率
 */
#define Set_SuperCap_CompensationLimit(handle, compensation_limit) (handle->config_value.power_compensation_limit = (compensation_limit))

/**
 * @brief 设置电容充电功率
 * @param[out] handle 超级电容句柄
 * @param[in] charge_limit 最大充电功率,超级电容充电的最大功率
 */
#define Set_SuperCap_ChargeLimit(handle, charge_limit) (handle->config_value.power_charge_limit = (charge_limit))

/**
 * @brief 读取电容电压
 * @param[in] handle 超级电容句柄
 */
#define Get_SuperCap_Vcap(handle) (handle->feedbuck_value.Vcap)

/**
 * @brief 读取底盘功率
 * @param[in] handle 超级电容句柄
 */
#define Get_SuperCap_Pchassis(handle) (handle->feedbuck_value.Pchassis)

/**
 * @brief 读取底盘电压
 * @param[in] handle 超级电容句柄
 */
#define Get_SuperCap_Vchassis(handle) (handle->feedbuck_value.Vchassis)

/**
 * @brief 读取充电功率
 * @param[in] handle 超级电容句柄
 */
#define Get_SuperCap_Pcharge(handle) (handle->feedbuck_value.Pcharge)

/**
 * @brief 读取电容状态
 * @param[in] handle 超级电容句柄
 * @param flag_name 状态名称，例如获取电容使能情况：Get_SuperCap_Status(CapEnable)
 */
#define Get_SuperCap_Status(handle,flag_name) (handle->status.flag.##flag_name)

/**
 * @brief 超级电容失联检测
 * @param[in] handle 超级电容句柄
 * @return 失联返回1,没有返回0
 */
#define Get_SuperCap_Loss_Of_Connection(handle) (SEML_GetTick() - handle->rx_timestamp >= SUPERCAP_CONNECT_TIMEOUT?1:0)

/**
 * @brief 超级电容句柄初始化
 * @param[out] handle 超级电容句柄
 * @param[in] can_handle can句柄
 */
void SuperCap_Init(SuperCap_t *handle, Can_Handle_t *can_handle);

/**
 * @brief 超级电容接收回调函数
 * @note 解算超级电容接收值
 * @param[out] handle 超级电容回调函数
 * @param[in] pack 接收数据包
*/
void SuperCap_Rx_Callback(SuperCap_t *handle, message_Pack_t const *const pack);

/**
 * @brief 向超级电容发送配置
 * @note 定期调用(最佳为10ms发送一次),以便及时更新超级电容配置信息
 * @param[in] handle 超级电容句柄
 */
void SuperCap_Sand_Config(SuperCap_t *handle);

/**
 * @brief 向超级电容发送裁判系统缓冲数据
 * @param[in] handle 超级电容句柄
 * @param[in] buffer_power 裁判系统发来的能量缓冲
 */
void SuperCup_Sand_Data(SuperCap_t *handle, float buffer_power);

#endif

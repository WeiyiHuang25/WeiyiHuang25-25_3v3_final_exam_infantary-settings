/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : u8g2_config.h
 * @author : SY7_yellow
 * @brief  : u8g2配置函数
 * @date   : 2023-7-17
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-07-19     <td>0.1         <td>SY7_yellow  <td>创建初始版本
 * </table>
 *
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 见u8g2.md文件
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
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 *
 ******************************************************************************
 */

#ifndef __U8G2_CONFIG_h_
#define __U8G2_CONFIG_h_

#include "main.h"
#include "u8g2.h"
#ifdef USE_SEML_LIB
#include "i2c_if.h"
/**
 * @brief 初始化使用软件i2c的u8g2
 */
#define u8g2_sw_i2c_init(u8g2_handle, hi2c, u8g2_setup)                                \
	do                                                                                   \
	{                                                                                    \
		Software_I2C_Register(&(u8g2_handle)->u8x8.I2C_Handle, (hi2c));                   \
		u8g2_setup(u8g2_handle, U8G2_R0, u8x8_byte_seml_i2c, u8g2_gpio_and_delay_seml); \
		u8g2_InitDisplay(u8g2_handle);                                                     \
		u8g2_SetPowerSave(u8g2_handle, 0);                                                 \
		u8g2_ClearDisplay(u8g2_handle);                                                    \
		u8g2_ClearBuffer(u8g2_handle);                                                     \
	} while (0)
/**
 * @brief 初始化使用硬件i2c的u8g2
 */
#define u8g2_hw_i2c_init(u8g2_handle, hi2c, u8g2_setup)                                \
	do                                                                                   \
	{                                                                                    \
		Hardware_I2C_Register(&(u8g2_handle)->u8x8.I2C_Handle, (hi2c));                   \
		u8g2_setup(u8g2_handle, U8G2_R0, u8x8_byte_seml_i2c, u8g2_gpio_and_delay_seml); \
		u8g2_InitDisplay(u8g2_handle);                                                     \
		u8g2_SetPowerSave(u8g2_handle, 0);                                                 \
		u8g2_ClearDisplay(u8g2_handle);                                                    \
		u8g2_ClearBuffer(u8g2_handle);                                                     \
	} while (0)
#endif
/// @brief u8g2通讯回调函数
/// @param u8x8 u8g2配置结构体
/// @param msg 指令类型
/// @param arg_int 数据大小
/// @param arg_ptr 数据指针
/// @return 运行状态
/// @retval 0 没有该指令
/// @retval 1 执行完成
uint8_t u8x8_byte_seml_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/// @brief u8g2延时回调函数
/// @param u8x8 u8g2配置结构体
/// @param msg 指令类型
/// @param arg_int 数据大小
/// @param arg_ptr 数据指针
/// @return 运行状态
/// @retval 0 没有该指令
/// @retval 1 执行完成
uint8_t u8g2_gpio_and_delay_seml(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
#endif

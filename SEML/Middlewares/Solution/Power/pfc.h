/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : pfc.h
  * @author : SY7_yellow
  * @brief  : 功率因数矫正模块
  * @date   : 2023-7-28
  * @par 修改日志：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-07-28     <td>0.1-alpha   <td>SY7_yellow  <td>创建初始版本
  * </table>
  * @todo    : 使用q16格式，尽量摆脱使用浮点型
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 1. 
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
  ******************************************************************************
  */
#ifndef _PFC_H_
#define _PFC_H_
#include "SEML_common.h"
#include "math_common.h"
#include "PID.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 电源控制库
 * @{
 */
/**
 * @brief PFC配置结构体
 */
typedef struct 
{
  PIDConfig_t Iloop_PID;    /**< 电流环的PID配置 */
  PIDConfig_t Vloop_PID;    /**< 电压环的PID配置 */

  struct
  {
    float expect_out_V;     /**< 目标输出电压 */
    float boost_min_duty;   /**< boost最小占空比 */
  } data;

  struct
  {
    float boost_min_duty;   /**< 最小boost占空比 */
    float sample_frec;      /**< 采样频率 */
    float vin_gain;         /**< 输入增益 */
  } config_data;

  struct
  {
    float setting_max_I;    /**< 设定最大电流 */
    float setting_out_V;    /**< 设定输出电压 */
  } setting_data;
}PFC_Config_t;

/// @brief PFC初始化
/// @param Power_Config 电源环路配置指针
/// @param Vin_m 输入最大值(V,安)
/// @param max_transform_ratio 最大变压比,调试时候可以给小，在正式使用时候给0
/// @param control_frec 控制频率
/// @param ILoop_Param 电流环pid参数
/// @param VLoop_Param 电压环pid参数
void PFC_Init(PFC_Config_t *Power_Config, float Vin_m, float max_transform_ratio, float control_frec, PID_Param_t ILoop_Param, PID_Param_t VLoop_Param);

/**
 * @brief pfc参数配置
 * @param Power_Config 电源环路配置指针
 * @param Vset 设定电压(V,伏)
 * @param Imax 最大电流(A,安)
 */
void PFC_Config(PFC_Config_t *Power_Config, float Vset, float Imax);

/**
 * @brief 功率因数矫正环路控制
 * @param[in,out] Power_Config 电源环路配置指针
 * @param[in] Vout 输出电压(V,伏)
 * @param[in] Vin 输入电压(V,伏),需滤除直流分量
 * @param[in] IL 电感电流(A,安)
 * @return boost占空比(1-D,上管占空比)
 */
_FAST float PFC_Loop_Control(PFC_Config_t *Power_Config, float Vout, float Vin, float IL);
/// @} 电源控制库结束
/// @} 解算库结束
#endif
/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : pll.h
  * @author : SY7_yellow
  * @brief  : 锁相环模块
  * @date   : 2023-7-15
  * @par 修改日志：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-07-15     <td>0.1-alpha   <td>SY7_yellow  <td>实现了SRF-PLL
  * </table>
  * @todo    : 使用q16格式，尽量摆脱使用浮点型
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 1. 若知道电压峰值可以使用SRF_PLL_Init()进行初始化,通过调节调节时间来计算最优
  *    PI参数.
  * 2. 若已经有计算好的参数可以使用SRF_PLL_Config()直接对PI参数进行调节.
  * 3. 在环路控制中调用SRF_PLL()即可.
  * 4. SRF_PLL传入的电压其直流分量需消去，例如使用一个高通滤波器滤除小于基波频率的
  *    分量.若其波形非正弦波其锁相效果也会变差。这时需要经过一个低通滤波器滤出高频
  *    分量。
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
#ifndef _PLL_H_
#define _PLL_H_
#include "SEML_common.h"
#include "foc.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 电源控制库
 * @{
 */

/**
 * @addtogroup 锁相环
 * @{
 */
/**
 * @addtogroup 同步坐标系锁相环
 * @{
 */
/**
 * @brief 同步坐标系锁相环配置结构体
 */
typedef struct 
{
    float LF_P;         // 低通滤波器比例系数
    float LF_I;         // 低通滤波器积分系数
    float LF_integral;  // 低通滤波器积分值
    float theta_hat;    // 估计相位
    float w_hat;        // 估计角速度
    dq_frame_t Udq;     // 估测出来的dq轴电压
}SRF_PLL_Config_t;

/**
 * @brief 同步坐标系锁相环初始化
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] Vm 交流电压幅值(V)
 * @param[in] Ts 调节时间
*/
void SRF_PLL_Init(SRF_PLL_Config_t *SRF_PLL_Config,const float Vm,const float Ts);

/**
 * @brief 同步坐标系锁相环参数配置
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] LF_P 低通滤波器比例系数
 * @param[in] LF_I 低通滤波器积分系数
*/
void SRF_PLL_Config(SRF_PLL_Config_t *SRF_PLL_Config,const float LF_P,const float LF_I);

/**
 * @brief 同步坐标系锁相环
 * SRF_PLL适用于无直流分量的对称三相电压的锁相.
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] Uab α-β轴电压(V)
 * @param[in] sample_time 采样时间(s)
 * @return 预估相位(rad)
 * @note 如需要读取Udq和ω请直接在配置结构体里面读取
*/
float SRF_PLL(SRF_PLL_Config_t *SRF_PLL_Config,alpha_beta_frame_t Uab,float sample_time);
//@} 同步坐标系锁相环结束

/**
 * @addtogroup 二阶广义正交信号发生器
 * @{
 */

typedef struct 
{
  float K;            /**< 阻尼系数 */
  float integral[2];  /**< 积分 */
} SOGI_QSG_Config_t;

/**
 * @brief 二阶广义正交信号发生器初始化
 * @param[out] SOGI_QSG_Config 二阶广义正交信号发生器配置结构体指针
 * @param[in] K 阻尼系数
 */
void SOGI_QSG_Init(SOGI_QSG_Config_t *SOGI_QSG_Config,float K);

/**
 * @brief 二阶广义正交信号发生器
 * @param[in,out] SOGI_QSG_Config 二阶广义正交信号发生器配置结构体
 * @param[out] Uab 输出的αβ轴正交电压(V),输入电压位于α,生成的正交电压位于β.
 * @param[in] Vin 输入电压(V)
 * @param[in] w SOGI谐振角速度(输入电压角速度)(rad/s)
 * @param[in] sample_time 采样时间(s)
 */
_FAST void SOGI_QSG(SOGI_QSG_Config_t *SOGI_QSG_Config,alpha_beta_frame_t *Uab,float Vin,float w, float sample_time);

/// @} 二阶广义正交信号发生器结束
/// @} 锁相环结束
/// @} 电源控制库结束
/// @} 解算库结束
#endif
/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : math_quaternion.h
  * @author : SY7_yellow
  * @brief  : 四元数运算模块
  * @date   : 2023-9-27
  * @par Change Log：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-9-27      <td>0.1-alpha   <td>SY7_yellow  <td>创建初始版本
  * </table>
  *@details :
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 
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
  * 优先兼容arm_math.h的格式，使用第三方的库的时候注意检查。\n
  * 浮点类型固定为float,如果需要使用double请调用"math.h"或使用其他第三方库。
  * (Cortex-M系列的fpu只有硬件单精度计算，双精度的依然是软件计算)\n
  * File encoding:UTF-8,Do not upload garbled code!\n
  * Please remember to change the version number. If you have not been tested, 
  * please use the suffix alpha,beta... And do not merge to master. \n
  * Defensive programming, where input arguments are checked for validity and
  * an error number is returned. \n
  * Don't include too many irrelevant headers. \n
  * If you find a bug, file an issue with a detailed description of the 
  * phenomenon (required) and conditions for reoccurrence (optional) so that 
  * the appropriate owner can locate the correct fix. \n
  * Compatibility with arm_math.h format is preferred. When using third-party 
  * libraries, pay attention to check. \n
  * The floating-point type is fixed to float, so if you need to use double call
  * "math.h" or use another third-party library. 
  * (The Cortex-M fpu only has hardware single precision calculation, the double
  * precision is still software calculation)\n
  ******************************************************************************
  */
#ifndef _MATH_QUATERNOIN_H_
#define _MATH_QUATERNOIN_H_
#include "math_common.h"
/**
 * @addtogroup 数学库
 * @{
 */
/**
 * @addtogroup 四元数运算模块
 * 常用的四元数运算
 * @{
 */
/**
 * @brief 四元数加法
 * 计算qr = qa + qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Add(const float *qa, const float *qb, float *qr);

/**
 * @brief 四元数减法
 * 计算qr = qa - qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Sub(const float *qa, const float *qb, float *qr);

/**
 * @brief 四元数减法
 * 计算qr = qa - qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Sub(const float *qa, const float *qb, float *qr);

/**
 * @brief 四元数乘法
 * 计算qr = qa * qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Mul(const float *qa, const float *qb, float *qr);

/**
 * @brief 求共轭四元数
 * 计算qr = qa*
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Conj(const float *qa, float *qr);

/**
 * @brief 求四元数的逆
 * 计算qa^-1
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
*/
void Quaternion_Inv(const float *qa, float *qr);

/**
 * @brief 将四元数归一化
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
 * @return qa的模长
*/
float Quaternion_Normalization(const float *qa, float *qr);

/**
 * @brief 求四元数的模
 * 计算|qa|
 * @param[in] qa 四元数数组
 * @return |qa|的结果
*/
float Quaternion_Norm(const float *qa);
/**
 * @} 四元数运算模块
 */
/**
 * @} 数学库末尾
 */

#endif

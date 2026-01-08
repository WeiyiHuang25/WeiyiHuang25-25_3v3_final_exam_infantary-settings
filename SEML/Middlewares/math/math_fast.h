/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : math_fast.h
  * @author : SY7_yellow
  * @brief  : 快速数学库
  * @date   : 2023-7-6
  * @par Change Log：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-7-6     <td>0.1-alpha   <td>SY7_yellow  <td>创建初始版本
  * <tr><td>2023-7-8     <td>0.1-beta    <td>SY7_yellow  <td>用标准库替代比标准库慢的函数
  * <tr><td>2023-8-4     <td>0.2         <td>SY7_yellow  <td>添加了反正切函数
  * </table>
  *@details :
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 1. 直接调用对应函数即可，若不满足精度要求可以瞄一眼"math_common.h"。
  * 2. 误差小于0.01%不标注误差情况。\n
  * 3. 在运算出现错误时候默认返回0,并且置math_errno为对应错误标志位。
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
#ifndef _MATH_FAST_H_
#define _MATH_FAST_H_
#include "SEML_common.h"
#include "math_common.h"
/**
 * @addtogroup 数学库
 * @{
 */
/**
  * @addtogroup 快速数学函数
  * 此类函数为了满足快速计算需求的常用函数，其运算量小，精度通常较低，
  * 对精度要求不高时可以使用该组函数提高性能。
  * @{
 */

/**
 * @addtogroup 指数对数函数
 * @{
 */
/**
 * @brief 快速exp
 * @param[in] x 输入值
 * @return e^x的估算值(±4%误差)
 * @see https://www.796t.com/content/1550634858.html
 */
_FAST float fast_exp(float x);

/**
 * @brief 快速自然对数
 * @param[in] x 输入值
 * @return lnx的估算值(±6%误差)
 */
_FAST float fast_ln(float x);

/**
 * @} 指数对数函数末尾
 */
/**
 * @addtogroup 反三角函数
 * @{
*/
/**
 * @brief 快速正切计算
 * @param[in] x 输入值
 * @return arctan估算值(0.1%误差)
 */
float fast_atan(float x);

/**
 * @brief 快速正切计算(全象限)
 * @param[in] x cosx值
 * @param[in] y sinx值
 * @return arctan估算值(0.1%误差)
 */
float fast_atan2(float y,float x);
/**
 * @} 反三角函数末尾
 */
/**
 * @} 快速数学函数末尾
 */
/**
 * @} 数学库末尾
*/
#endif

// /**
//   ******************************************************************************
//   * @copyright (c) 2023 - ~, Singularity
//   * @file   : math_matrix.h
//   * @author : SY7_yellow
//   * @brief  : 矩阵运算库
//   * @date   : 2023-7-6
//   * @par Change Log：
//   * <table>
//   * <tr><th>Date           <th>Version     <th>Author      <th>Description
//   * <tr><td>2023-7-6     <td>0.1-alpha   <td>SY7_yellow  <td>初始arm_math.h矩阵运算的支持
//   * </table>
//   *@details :
//   * ============================================================================
//   *                       How to use this driver  
//   * ============================================================================
//   * 1. 直接调用对应函数即可，若需要快速运算可以瞄一眼"math_fast.h"。
//   * 2. 在运算出现错误时候默认返回0,并且置math_errno为对应错误标志位。
//   ******************************************************************************
//   * @attention:
//   * 
//   * 文件编码：UTF-8,出现乱码请勿上传! \n
//   * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
//   * 勿合并到master. \n
//   * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
//   * 不要包含太多不相关的头文件. \n
//   * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
//   * 够准确定位修复. \n
//   * 优先兼容arm_math.h的格式，使用第三方的库的时候注意检查。\n
//   * 浮点类型固定为float,如果需要使用double请调用"math.h"或使用其他第三方库。
//   * (Cortex-M系列的fpu只有硬件单精度计算，双精度的依然是软件计算)\n
//   * File encoding:UTF-8,Do not upload garbled code!\n
//   * Please remember to change the version number. If you have not been tested, 
//   * please use the suffix alpha,beta... And do not merge to master. \n
//   * Defensive programming, where input arguments are checked for validity and
//   * an error number is returned. \n
//   * Don't include too many irrelevant headers. \n
//   * If you find a bug, file an issue with a detailed description of the 
//   * phenomenon (required) and conditions for reoccurrence (optional) so that 
//   * the appropriate owner can locate the correct fix. \n
//   * Compatibility with arm_math.h format is preferred. When using third-party 
//   * libraries, pay attention to check. \n
//   * The floating-point type is fixed to float, so if you need to use double call
//   * "math.h" or use another third-party library. 
//   * (The Cortex-M fpu only has hardware single precision calculation, the double
//   * precision is still software calculation)\n
//   ******************************************************************************
//   */

// #ifndef _MATH_MATRIX_H_
// #define _MATH_MATRIX_H_
// #include "SEML_common.h"
// #include "math_common.h"

// /**
//  * @brief 矩阵结构体定义
//  * @details 该结构指定矩阵的大小，然后指向数据数组。数组的大小和值按行顺序排列。
//  * 也就是，矩阵元素 （i， j） 存储在：numRows * numCols： pData[i*numCols + j]
//  */
// #ifdef _ARM_MATH_H
// typedef arm_matrix_instance_f32 matrix_t;
// #else
// typedef struct
// {
//    uint16_t numRows;     /**< 矩阵行个数     */
//    uint16_t numCols;     /**< 矩阵列个数     */
//    float *pData; /**< 矩阵数据指针   */
// } matrix_t;
// #endif
// ///@brief 矩阵库断言函数
// #ifdef USE_MATH_ASSERT
// #define matirx_assert_param(expr)         \
//    if (!expr)                             \
//    {                                      \
//       math_errno = MATH_PARAMETERS_ERROR; \
//       return INPUT_PARAMETERS_ERROR;      \
//    }
// #else
// #define matirx_assert_param(expr)        ((void)0U)
// #endif
// /**
//  * @brief 矩阵运算状态枚举结构体
//  * @details 该结构体作为矩阵返回值输出，说明矩阵运算状态。
//  * 返回值非0且不为1时候会置math_errno为MATRIX_ERROR。
//  * 返回值为1时候会置math_errno为MATH_PARAMETERS_ERROR。
//  */
// typedef enum
// {
//    MATRIX_NO_ERROR = 0,                          /**< 无错误                                  */
//    INPUT_PARAMETERS_ERROR,                       /**< 输入参数错误                            */
//    MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL,             /**< 矩阵行列不相等                          */
//    MATRIX_MULTIPLICATION_ERROR,                  /**< 矩阵乘法错误(左矩阵列不等于右矩阵行)    */
//    MATRIX_MUST_BE_SQUARE,                        /**< 矩阵必须为方阵                          */
//    MATRIX_EQUATION_HAS_NO_SOLUTIONS,             /**< 矩阵方程无解                            */
//    MATRIX_EQUATION_HAS_INFINITY_MANNY_SOLUTIONS, /**< 矩阵方程有无穷多解                      */
//    CREATE_MATRIX_FAILED,                         /**< 创建矩阵失败                            */
//    MATRIX_TRANSPOSE_FAILED                       /**< 矩阵转置失败                            */

// } matrix_error_t;

// /**
//  * @addtogroup 数学库
//  * @{
//  */
// /**
//  * @addtogroup 矩阵运算函数
//  * 常用的矩阵运算
//  * @{
//  */
// /**
//  * @brief 矩阵初始化
//  * @param[out] pMat 矩阵结构体指针
//  * @param[in] row 行个数
//  * @param[in] column 列个数
//  * @param[in] pData 数据指针
//  * @return 矩阵运算状态 
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval CREATE_MATRIX_FAILED 创建矩阵出错
//  */
// matrix_error_t matrix_init(matrix_t *pMat, const uint16_t row, const uint16_t column, float *pData);

// /**
//  * @brief 矩阵加法，计算A+B=C
//  * @param[in] pMatA 输入矩阵A
//  * @param[in] pMatB 输入矩阵B
//  * @param[out] pMatC 输出矩阵C
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL 行或列不相等
// */
// _FAST matrix_error_t matrix_add(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC);

// /**
//  * @brief 矩阵减法，计算A-B=C
//  * @param[in] pMatA 输入矩阵A
//  * @param[in] pMatB 输入矩阵B
//  * @param[out] pMatC 输出矩阵C
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL 行或列不相等
// */
// _FAST matrix_error_t matrix_sub(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC);

// /**
//  * @brief 矩阵乘法，计算A*B=C
//  * @param[in] pMatA 输入矩阵A
//  * @param[in] pMatB 输入矩阵B
//  * @param[out] pMatC 输出矩阵C
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_MULTIPLICATION_ERROR 矩阵乘法错误
// */
// _FAST matrix_error_t matrix_mult(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC);

// /**
//  * @brief 矩阵的逆，计算A^-1=B
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_MUST_BE_SQUARE 矩阵必须是方阵
//  * @retval MATRIX_EQUATION_HAS_NO_SOLUTIONS 矩阵方程无解
// */
// _FAST matrix_error_t matrix_inverse(const matrix_t *pMatA, matrix_t *pMatB);

// /**
//  * @brief 矩阵的转置，计算A^T=B
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_TRANSPOSE_FAILED 矩阵转置失败
// */
// _FAST matrix_error_t matrix_trans(const matrix_t *pMatA, matrix_t *pMatB);

// /**
//  * @brief 矩阵的缩放，计算kA=B
//  * @param[in] k 缩放系数
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL 行或列不相等
// */
// _FAST matrix_error_t matrix_scale(const float k, const matrix_t *pMatA, matrix_t *pMatB);

// /**
//  * @brief 矩阵的行列式，计算|A|=y
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] y 行列式结果
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_MUST_BE_SQUARE 矩阵必须是方阵
//  * @todo 实现矩阵行列式计算
// */
// _FAST matrix_error_t matrix_determinant(const matrix_t *pMatA, float *y);
// /**
//  * @} 矩阵运算函数末尾
//  */
// /**
//  * @} 数学库末尾
//  */

// #endif
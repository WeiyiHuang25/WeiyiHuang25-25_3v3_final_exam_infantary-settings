// #include "math_matrix.h"
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
// matrix_error_t matrix_init(matrix_t *pMat, const uint16_t row, const uint16_t column, float *pData)
// {
//     uint16_t temp = row * column;
//     matirx_assert_param(IS_NULL_POINT(pMat));
//     matirx_assert_param(IS_NULL_POINT(pData));
	
//     // 行列为0报错
//     if (temp == 0)
//     {
//         math_errno = MATRIX_ERROR;
//         return CREATE_MATRIX_FAILED;
//     }

//     pMat->numCols = column;
//     pMat->numRows = row;
//     pMat->pData = pData;

//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_determinant(const matrix_t *pMatA, float *y)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(y));

//     //输入部位方阵
//     if (pMatA->numRows != pMatA->numCols)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_MUST_BE_SQUARE;
//     }



//     return MATRIX_NO_ERROR;
// }

// #ifdef _ARM_MATH_H_
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
// matrix_error_t matrix_add(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));
//     if (arm_mat_add_f32(pMatA, pMatB, pMatC))
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }
//     return MATRIX_NO_ERROR;
// }
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
// matrix_error_t matrix_sub(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));
//     if (arm_mat_sub_f32(pMatA, pMatB, pMatC))
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }
//     return MATRIX_NO_ERROR;
// }


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
// matrix_error_t matrix_mult(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));
//     // 输出矩阵不相等
//     if (arm_mat_mult_f32(pMatA, pMatB, pMatC))
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_MULTIPLICATION_ERROR;
//     }
//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_inverse(const matrix_t *pMatA, matrix_t *pMatB)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     switch (arm_mat_inverse_f32(pMatA, pMatB))
//     {
//     case ARM_MATH_SIZE_MISMATCH:
//         // 矩阵大小错误
//         math_errno = MATRIX_ERROR;
//         return MATRIX_MUST_BE_SQUARE;
//     case ARM_MATH_SINGULAR:
//         // 矩阵为奇异矩阵
//         math_errno = MATRIX_ERROR;
//         return MATRIX_EQUATION_HAS_NO_SOLUTIONS;
//     }
//     return MATRIX_NO_ERROR;
// }

// /**
//  * @brief 矩阵的转置，计算A^T=B
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_TRANSPOSE_FAILED 矩阵转置失败
// */
// matrix_error_t matrix_trans(const matrix_t *pMatA, matrix_t *pMatB)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     if (arm_mat_trans_f32(pMatA, pMatB))
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_TRANSPOSE_FAILED;
//     }
//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_scale(const float k, const matrix_t *pMatA, matrix_t *pMatB)
// {
//     matirx_assert_param(!IS_NAN_INF(k));
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
    
//     if (arm_mat_scale_f32(pMatA, k, pMatB))
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }
//     return MATRIX_NO_ERROR;
// }

// #else

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
// matrix_error_t matrix_add(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     uint16_t i, j;
//     uint16_t rows, columns;
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));

//     if (pMatA->numRows != pMatB->numRows || pMatA->numRows != pMatC->numRows || pMatB->numRows != pMatC->numRows \
//      || pMatA->numCols != pMatB->numCols || pMatA->numCols != pMatC->numCols || pMatB->numCols != pMatC->numCols)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }

//     rows = pMatA->numRows;
//     columns = pMatB->numCols;
//     for (i = 0; i < rows; i++)
//     {
//         for (j = 0; j < columns; j++)
//         {
//             pMatC->pData[i * columns + j] = pMatA->pData[i * columns + j] + pMatB->pData[i * columns + j];
//         }
//     }

//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_sub(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     uint16_t i, j;
//     uint16_t rows, columns;
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));

//     //矩阵行列不相等
//     if (pMatA->numRows != pMatB->numRows || pMatA->numRows != pMatC->numRows || pMatB->numRows != pMatC->numRows \
//      || pMatA->numCols != pMatB->numCols || pMatA->numCols != pMatC->numCols || pMatB->numCols != pMatC->numCols)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }

//     rows = pMatA->numRows;
//     columns = pMatB->numCols;
//     for (i = 0; i < rows; i++)
//     {
//         for (j = 0; j < columns; j++)
//         {
//             pMatC->pData[i * columns + j] = pMatA->pData[i * columns + j] - pMatB->pData[i * columns + j];
//         }
//     }

//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_mult(const matrix_t *pMatA, const matrix_t *pMatB, matrix_t *pMatC)
// {
//     uint16_t i, j, k;
//     float sum;

//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     matirx_assert_param(IS_NULL_POINT(pMatC));

//     //左矩阵行不等于右矩阵列
//     if (pMatA->numCols != pMatB->numRows || pMatA->numRows != pMatC->numRows || pMatB->numCols != pMatC->numCols)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_MULTIPLICATION_ERROR;
//     }

//     for (i = 0; i < pMatA->numRows; i++)
//     {
//         for (j = 0; j < pMatB->numCols; j++)
//         {
//             sum = 0.0;
//             for (k = 0; k < pMatA->numCols; k++)
//             {
//                 sum += pMatA->pData[i * pMatA->numCols + k] * pMatB->pData[k * pMatB->numCols + j];
//             }
//             pMatC->pData[i * pMatB->numCols + j] = sum;
//         }
//     }

//     return MATRIX_NO_ERROR;
// }

// /**
//  * @brief 矩阵的逆，计算A^-1=B
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_MUST_BE_SQUARE 矩阵必须是方阵
//  * @retval MATRIX_EQUATION_HAS_NO_SOLUTIONS 矩阵方程无解
//  * @todo 使用软件方法实现
// */
// matrix_error_t matrix_inverse(const matrix_t *pMatA, matrix_t *pMatB)
// {
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));
//     switch (arm_mat_inverse_f32(pMatA, pMatB))
//     {
//     case ARM_MATH_SIZE_MISMATCH:
//         // 矩阵大小错误
//         math_errno = MATRIX_ERROR;
//         return MATRIX_MUST_BE_SQUARE;
//     case ARM_MATH_SINGULAR:
//         // 矩阵为奇异矩阵
//         math_errno = MATRIX_ERROR;
//         return MATRIX_EQUATION_HAS_NO_SOLUTIONS;
//     }
//     return MATRIX_NO_ERROR;
// }

// /**
//  * @brief 矩阵的转置，计算A^T=B
//  * @param[in] pMatA 输入矩阵A
//  * @param[out] pMatB 输出矩阵B
//  * @return 矩阵运算状态
//  * @retval MATRIX_NO_ERROR 无错误
//  * @retval INPUT_PARAMETERS_ERROR 输入参数错误
//  * @retval MATRIX_TRANSPOSE_FAILED 矩阵转置失败
// */
// matrix_error_t matrix_trans(const matrix_t *pMatA, matrix_t *pMatB)
// {
//     uint16_t i, j;

//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));

//     // 输入输出矩阵行列不相等
//     if (pMatA->numRows != pMatB->numCols || pMatA->numCols != pMatB->numRows)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_TRANSPOSE_FAILED;
//     }

//     for (i = 0; i < pMatA->numRows; i++)
//     {
//         for (j = 0; j < pMatA->numCols; j++)
//         {
//             pMatB->pData[j * pMatA->numRows + i] = pMatA->pData[i * pMatA->numCols + j];
//         }
//     }

//     return MATRIX_NO_ERROR;
// }

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
// matrix_error_t matrix_scale(const float k, const matrix_t *pMatA, matrix_t *pMatB)
// {
//     uint16_t i, j;

//     matirx_assert_param(!IS_NAN_INF(k));
//     matirx_assert_param(IS_NULL_POINT(pMatA));
//     matirx_assert_param(IS_NULL_POINT(pMatB));

//     //输入行不等于输出列，输入列不等于输出行
//     if (pMatA->numRows != pMatB->numRows || pMatA->numCols != pMatB->numCols)
//     {
//         math_errno = MATRIX_ERROR;
//         return MATRIX_ROWS_OR_COLUMNS_NOT_EQUAL;
//     }

//     for (i = 0; i < pMatA->numRows; i++)
//     {
//         for (j = 0; j < pMatA->numCols; i++)
//         {
//             pMatB->pData[i * pMatA->numCols + j] = k * pMatA->pData[i * pMatA->numCols + j];
//         }
//     }

//     return MATRIX_NO_ERROR;
// }

// #endif
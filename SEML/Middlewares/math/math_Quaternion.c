#include "math_quaternion.h"
/**
 * @brief 四元数加法
 * 计算qr = qa + qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
inline void Quaternion_Add(const float *qa, const float *qb, float *qr)
{
  qr[0] = qa[0] + qb[0];
  qr[1] = qa[1] + qb[1];
  qr[2] = qa[2] + qb[2];
  qr[3] = qa[3] + qb[3];
}

/**
 * @brief 四元数减法
 * 计算qr = qa - qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
inline void Quaternion_Sub(const float *qa, const float *qb, float *qr)
{
  qr[0] = qa[0] - qb[0];
  qr[1] = qa[1] - qb[1];
  qr[2] = qa[2] - qb[2];
  qr[3] = qa[3] - qb[3];
}

/**
 * @brief 求四元数的模
 * 计算|qa|
 * @param[in] qa 四元数数组
 * @return |qa|的结果
*/
inline float Quaternion_Norm(const float *qa)
{
  return math_sqrt(qa[0] * qa[0] + qa[1] * qa[1] + qa[2] * qa[2] + qa[3] * qa[3]);
}

/**
 * @brief 四元数乘法
 * 计算qr = qa * qb
 * @param[in] qa 四元数数组
 * @param[in] qb 四元数数组
 * @param[out] qr 四元数数组
*/
inline void Quaternion_Mul(const float *qa, const float *qb, float *qr)
{
  qr[0] = qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2] - qa[3] * qb[3];
  qr[1] = qa[0] * qb[1] + qa[1] * qb[0] + qa[2] * qb[3] - qa[3] * qb[2];
  qr[2] = qa[0] * qb[2] + qa[2] * qb[0] + qa[3] * qb[1] - qa[1] * qb[3];
  qr[3] = qa[0] * qb[3] + qa[3] * qb[1] + qa[1] * qb[2] - qa[2] * qb[1];
}

/**
 * @brief 求共轭四元数
 * 计算qr = qa*
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
*/
inline void Quaternion_Conj(const float *qa, float *qr)
{
  qr[0] = +qa[0];
  qr[1] = -qa[1];
  qr[2] = -qa[2];
  qr[3] = -qa[3];
}

/**
 * @brief 求四元数的逆
 * 计算qa^-1
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
*/
inline void Quaternion_Inv(const float *qa, float *qr)
{
  float temp;
  Quaternion_Conj(qa,qr);
  temp = 1.0f / Quaternion_Norm(qa);
  temp *= temp;
  qr[0] *= temp;
  qr[1] *= temp;
  qr[2] *= temp;
  qr[3] *= temp;
}

/**
 * @brief 将四元数归一化
 * @param[in] qa 四元数数组
 * @param[out] qr 四元数数组
 * @return qa的模长
*/
inline float Quaternion_Normalization(const float *qa, float *qr)
{
  float norm = Quaternion_Norm(qa);
  float inv_norm = 1.0f / norm;
  qr[0] = qa[0] * inv_norm;
  qr[1] = qa[1] * inv_norm;
  qr[2] = qa[2] * inv_norm;
  qr[3] = qa[3] * inv_norm;
  return norm;
}


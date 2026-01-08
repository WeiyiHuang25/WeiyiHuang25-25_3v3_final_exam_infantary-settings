//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h
#include "AHRS.h"
/**
 * @addtogroup 解算库
 * @{
*/
/**
 * @addtogroup 航姿参考系统解算模块
 * @{
*/

/**
 * @brief AHRS解算更新(基于Madgwick算法)
 * @param[in,out] quat 四元数
 * @param[in] sample_time 采样时间(S)
 * @param[in] accel 加速度(m/s^2)
 * @param[in] gyro 角加速度(rad/s)
 * @param[in] mag 磁场大小(uT)
 */
void Madgwick_AHRS_Update(float quat[4], float sample_time, Accel_Data_t *accel, Gyro_Data_t *gyro, Mag_Data_t *mag);

/**
 * @} 航姿参考系统解算模块
 */
/**
 * @} 解算库
*/
#endif

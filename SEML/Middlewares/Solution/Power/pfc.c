#include "pfc.h"

/// @brief PFC初始化
/// @param Power_Config 电源环路配置指针
/// @param Vin_m 输入最大值(V,安)
/// @param max_transform_ratio 最大变压比,调试时候可以给小，在正式使用时候给0
/// @param control_frec 控制频率
/// @param ILoop_Param 电流环pid参数
/// @param VLoop_Param 电压环pid参数
void PFC_Init(PFC_Config_t *Power_Config, float Vin_m, float max_transform_ratio, float control_frec, PID_Param_t ILoop_Param, PID_Param_t VLoop_Param)
{
	assert_param(Power_Config != NULL);

	Power_Config->setting_data.setting_max_I = 0;
	Power_Config->setting_data.setting_out_V = 0;

	Power_Config->data.boost_min_duty = 0.95f;
	Power_Config->data.expect_out_V = 0;

	Power_Config->config_data.sample_frec = 1.0f / control_frec;
	Power_Config->config_data.boost_min_duty = 1.0f / max_transform_ratio;
	Power_Config->config_data.vin_gain = 1.0f / Vin_m;

	PID_Init(&Power_Config->Iloop_PID, ILoop_Param.Kp, ILoop_Param.Ki, ILoop_Param.Kd, max_transform_ratio, 0, 10.0f, 1.0f / control_frec);
	PID_Init(&Power_Config->Vloop_PID, VLoop_Param.Kp, VLoop_Param.Ki, VLoop_Param.Kd, 10, 0, 10.0f, 1.0f / control_frec);
}

/**
 * @brief pfc参数配置
 * @param Power_Config 电源环路配置指针
 * @param Vset 设定电压(V,伏)
 * @param Imax 最大电流(A,安)
 */
void PFC_Config(PFC_Config_t *Power_Config, float Vset, float Imax)
{
	assert_param(Power_Config != NULL);

	Power_Config->setting_data.setting_out_V = Vset;
	Power_Config->setting_data.setting_max_I = Imax;
}

/**
 * @brief 功率因数矫正环路控制
 * @param[in,out] Power_Config 电源环路配置指针
 * @param[in] Vout 输出电压(V,伏)
 * @param[in] Vin 输入电压(V,伏),需滤除直流分量
 * @param[in] IL 电感电流(A,安)
 * @return boost占空比(1-D,上管占空比)
 */
float PFC_Loop_Control(PFC_Config_t *Power_Config, float Vout, float Vin, float IL)
{
	assert_param(Power_Config != NULL);
	float PIDoutput;
	float Vloop_out = 0, expect_i;
	Power_Config->data.boost_min_duty = Ramp_float(Power_Config->config_data.boost_min_duty, Power_Config->data.boost_min_duty, 0.001);
	// 电压环
	Vloop_out = Basic_PID_Controller(&Power_Config->Vloop_PID, Power_Config->data.expect_out_V, Vout);
	expect_i = Vloop_out * Vin * Power_Config->config_data.vin_gain;
	expect_i = min(expect_i, Power_Config->setting_data.setting_max_I);
	// 电流环
	PIDoutput = Basic_PI_Controller(&Power_Config->Iloop_PID, expect_i, IL);
	return constrain(PIDoutput, 1, Power_Config->data.boost_min_duty);
}

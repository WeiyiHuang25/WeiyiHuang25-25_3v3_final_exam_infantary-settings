#include "PID.h"
#include "math_filter.h"

/**
 * @brief PID配置初始化
 *     - 只配置最基础的pid参数
 *     - 抗积分饱和方式：动态钳位
 *     - 微分低通滤波:不滤波
 * @param[out] PID_Config PID配置结构体指针
 * @param[in] Kp 比例因子
 * @param[in] Ki 积分因子
 * @param[in] Kd 微分因子
 * @param[in] PID_Max 输出最大限幅值
 * @param[in] PID_Min 输出最小限幅值
 * @param[in] sample_time 采样时间(秒)
 */
void PID_Init(PIDConfig_t *PID_Config, PIDElem_t Kp, PIDElem_t Ki, PIDElem_t Kd, PIDElem_t PID_Max, PIDElem_t PID_Min, float sample_time)
{
	assert_param(sample_time > 0);
	assert_param(PID_Config != NULL);
	assert_param(PID_Max >= PID_Min);

	PID_Config->Param.Kp = Kp;
	PID_Config->Param.Ki = Ki * sample_time;
	PID_Config->Param.Kd = Kd / sample_time;
	PID_Config->PIDMax = PID_Max;
	PID_Config->PIDMin = PID_Min;
	PID_Config->SampTime = sample_time;

	PID_Config->ITerm = 0;
	PID_Config->lest_ErrValue = 0;
	// 积分抗饱和方式：积分遇限削弱法
	PID_Config->anti_windup = anti_windup_clamping;
	// 默认抗积分饱和系数:ki/kp
	PID_Config->Ki_Clamp = PID_Config->Param.Ki / PID_Config->Param.Kp;
	// 默认微分低通滤波系数:1 (不滤波)
	PID_Config->Kd_lowpass = 1;
	// 默认不使用回调
	PID_Config->Callback_Fun = NULL;
}

/**
 * @brief PID优化配置
 * @param[out] PID_Config PID配置结构体指针
 * @param[in] Ki_Clamp 抗积分饱和系数
 * @param[in] Kd_lowpass 微分低通滤波系数
 * @param[in] PID_Callback_Fun pid参数处理回调函数
 * @param[in] anti_windup 积分抗饱和方式
 */
void PID_Config(PIDConfig_t *PID_Config, PIDElem_t Ki_Clamp, const PIDElem_t Kd_lowpass, const PID_Callback_Fun_t PID_Callback_Fun, const PID_anti_windup_t anti_windup)
{
	assert_param(PID_Config != NULL);
	assert_param(Kd_lowpass >= 0 && Kd_lowpass <= 1);
	assert_param(Ki_Clamp >= 0);
	assert_param(IS_PID_ANTI_WINDUP(anti_windup));

	PID_Config->Ki_Clamp = Ki_Clamp;
	PID_Config->Kd_lowpass = Kd_lowpass;
	PID_Config->Callback_Fun = PID_Callback_Fun;
}

/**
 * @brief PID重置
 * @param[out] PID_Config PID配置结构体指针
 * @param[in] ITerm 积分值(乘以ki之后的)
 * @param[in] lest_ErrValue 上一次误差值
 */
void PID_Reset(PIDConfig_t *PID_Config, const PIDElem_t ITerm, const PIDElem_t lest_ErrValue)
{
	assert_param(PID_Config != NULL);

	PID_Config->ITerm = ITerm;
	PID_Config->lest_ErrValue = lest_ErrValue;
}

/**
 * @brief PID微分先行
 * @param[in,out] PIDConfig PID配置结构体指针
 * @param[in] E_value 期望值
 * @param[in] C_value 实际值
 */
void PID_Config_Differential_First(PIDConfig_t *PID_Config, const PIDElem_t E_value, const PIDElem_t C_value)
{
	PIDElem_t Itemp;

	// 积分
	Itemp = PID_Config->ErrValue * PID_Config->Param.Ki;
	PID_Config->ITerm += Itemp;
	// 微分
	PID_Config->DiffValue = Lowpass((C_value - PID_Config->lest_ErrValue) * PID_Config->Param.Kd, PID_Config->DiffValue, PID_Config->Kd_lowpass);
	PID_Config->lest_ErrValue = C_value;
}

/**
 * @brief 基础位置式PID控制器
 * @param[in,out] PIDConfig PID配置
 * @param[in] E_value 期望值
 * @param[in] C_value 实际值
 * @return 当前PID输出
 */
PIDElem_t Basic_PID_Controller(PIDConfig_t *PID_Config, const PIDElem_t E_value, const PIDElem_t C_value)
{
	PIDElem_t PIDtemp, Itemp;
	assert_param(PID_Config != NULL);

	// 当前误差
	PID_Config->ErrValue = E_value - C_value;
	// 计算积分微分
	if (PID_Config->Callback_Fun == NULL)
	{
		Itemp = PID_Config->ErrValue * PID_Config->Param.Ki;
		// 积分
		PID_Config->ITerm += Itemp;
		// 微分
		PID_Config->DiffValue = Lowpass((PID_Config->ErrValue - PID_Config->lest_ErrValue) * PID_Config->Param.Kd, PID_Config->DiffValue, PID_Config->Kd_lowpass);
		PID_Config->lest_ErrValue = PID_Config->ErrValue;
	}
	else
	{
		PID_Config->Callback_Fun(PID_Config, E_value, C_value);
	}
	// PID
	PIDtemp = PID_Config->Param.Kp * PID_Config->ErrValue + PID_Config->DiffValue + PID_Config->ITerm;
	// PID输出限制
	PID_Config->PIDout = Constrain_PID(PIDtemp, PID_Config->PIDMax, PID_Config->PIDMin);
	// 抗积分饱和
	switch (PID_Config->anti_windup)
	{
	case anti_windup_back_calculkation:
		PID_Config->ITerm += (PID_Config->PIDout - PIDtemp) * PID_Config->Ki_Clamp;
		break;
	case anti_windup_clamping:
		if (PIDtemp != PID_Config->PIDout && sign(PID_Config->ErrValue) * sign(PIDtemp) > 0)
		{
			PID_Config->ITerm -= Itemp;
		}
		break;
	}

	return PID_Config->PIDout;
}

/**
 * @brief 基础位置式PI控制器
 * @param[in,out] PIDConfig PID配置
 * @param[in] E_value 期望值
 * @param[in] C_value 实际值
 * @return 当前PID输出
 */
PIDElem_t Basic_PI_Controller(PIDConfig_t *PID_Config, const PIDElem_t E_value, const PIDElem_t C_value)
{
	PIDElem_t PIDtemp, Itemp;
	assert_param(PID_Config != NULL);
	assert_param(PID_Config->Param.Kd == 0);

	// 当前误差
	PID_Config->ErrValue = E_value - C_value;

	if (PID_Config->Callback_Fun == NULL)
	{
		Itemp = PID_Config->ErrValue * PID_Config->Param.Ki;
		// 积分
		PID_Config->ITerm += Itemp;
	}
	else
	{
		PID_Config->Callback_Fun(PID_Config, E_value, C_value);
	}
	// PID
	PIDtemp = PID_Config->Param.Kp * PID_Config->ErrValue + PID_Config->ITerm;
	// PID输出限制
	PID_Config->PIDout = Constrain_PID(PIDtemp, PID_Config->PIDMax, PID_Config->PIDMin);
	// 抗积分饱和
	switch (PID_Config->anti_windup)
	{
	case anti_windup_back_calculkation:
		PID_Config->ITerm += (PID_Config->PIDout - PIDtemp) * PID_Config->Ki_Clamp;
		break;
	case anti_windup_clamping:
		if (PIDtemp != PID_Config->PIDout && sign(PID_Config->ErrValue) * sign(PIDtemp) > 0)
		{
			PID_Config->ITerm -= Itemp;
		}
		break;
	}

	return PID_Config->PIDout;
}

/**
 * @brief 过零点处理
 * @param AngleMax 角度最大值,期望值和当前值范围为[0,AngleMax]
 * @param E_value 期望值
 * @param C_value 当前值
 * @return 零点处理后的期望值 
 */
PIDElem_t Zero_Crossing_Process(PIDElem_t AngleMax,PIDElem_t E_value, PIDElem_t C_value)
{
	PIDElem_t temp = E_value;
	//角度突变点处理
	if (abs(C_value - E_value) > AngleMax/2)
	{
		temp += (E_value > C_value) ? -AngleMax : AngleMax;
	}
	return temp;
}

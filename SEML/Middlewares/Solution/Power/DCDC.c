#include "DCDC.h"
/**
 * @brief 四开关buck-boost控制器初始化
 * @param[in,out] dcdc 电源环路句柄
 * @param[in] config 电源环路配置
 */
void BuckBoost_Init(BuckBoost_t *dcdc, BuckBoost_Config_t *config)
{
	assert_param(dcdc != NULL);
	assert_param(config != NULL);
	assert_param(config->max_transform_ratio >= 0);
	assert_param(config->V_ramp_ratio > 0);
	assert_param(config->control_frec > 0);
	assert_param(config->Iout != NULL);
	assert_param(config->Vout != NULL);
	dcdc->config = *config;
	dcdc->config.V_ramp_ratio /= config->control_frec / 100.0f;
	dcdc->count = 0;
	dcdc->expect_Vout = 0;
	dcdc->Set_Vout = 0;
	dcdc->Set_Iout = 0;
	dcdc->status = CV_Status;
	dcdc->mode = Wait_Mode;
	dcdc->Set_ShortCircuit = NULL;
}

/**
 * @brief 注册buck-boost设定短路电流回调函数
 * 该函数是设定模拟看门狗阈值的函数
 * @note 在模拟看门狗越界中断中调用BuckBoost_ShortCircuit_Handle进行短路处理
 * @param[in,out] dcdc 电源环路句柄
 * @param[in] Set_ShortCircuit 注册短路电流回调函数
 */
void BuckBoost_Register_Set_ShortCircuit_Callback(BuckBoost_t *dcdc, Set_ShortCircuit_Value_Fun_t Set_ShortCircuit)
{
	assert_param(dcdc != NULL);
	assert_param(Set_ShortCircuit != NULL);
	dcdc->Set_ShortCircuit = Set_ShortCircuit;
}
/**
 * @brief 开启四开关buck-boost环路
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Start(BuckBoost_t *dcdc)
{
	assert_param(dcdc != NULL);
	dcdc->expect_Vout = *dcdc->config.Vout;
	// 进行前馈
	if (dcdc->config.Vin != NULL)
		dcdc->config.Vloop_PID.u_1 = *dcdc->config.Vin > 0 ? *dcdc->config.Vout / *dcdc->config.Vin * 0.95f : 0;
	else
		dcdc->config.Vloop_PID.u_1 = 0;
	dcdc->config.Iloop_PID.u_1 = dcdc->config.max_transform_ratio;
	dcdc->mode = Buck_Mode;
	dcdc->status = CV_Status;
}

/**
 * @brief 关闭四开关buck-boost环路
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Stop(BuckBoost_t *dcdc)
{
	assert_param(dcdc != NULL);
	dcdc->expect_Vout = 0;
	dcdc->mode = Wait_Mode;
}

/**
 * @brief 设置四开关buck-boost的输出电压和输出电流
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Setting(BuckBoost_t *dcdc, float set_Vout, float set_Iout)
{
	assert_param(dcdc != NULL);
	assert_param(set_Vout >= 0);
	dcdc->Set_Vout = set_Vout;
	dcdc->Set_Iout = set_Iout;
	if (dcdc->Set_ShortCircuit != NULL)
		dcdc->Set_ShortCircuit(set_Iout * 1.5f);
}

/**
 * @brief 四开关buck-boost控制器短路处理函数
 * 当检测到输出电流超过阈值电流以后调用该函数
 * @param dcdc 电源环路句柄
 */
void BuckBoost_ShortCircuit_Handle(BuckBoost_t *dcdc)
{
	assert_param(dcdc != NULL);
	// 短路时关断电路,电流环从零开始启动.
	dcdc->config.Iloop_PID.u_1 = 0;
	*dcdc->config.boost_duty = dcdc->config.min_duty;
	*dcdc->config.buck_duty = dcdc->config.min_duty;
	if (dcdc->config.flag.shortcircuit_mode == SCP_Mode)
	{
		dcdc->status = SCP_Status;
		dcdc->mode = Wait_Mode;
	}
}

/**
 * @brief 四开关buck-boost电源环路控制
 * @param[in,out] dcdc 电源环路句柄
 */
void BuckBoost_Control_Loop(BuckBoost_t *dcdc)
{
	float PIDoutput;
	float Vloop_out = 0, Iloop_out = 0;
	float boost_duty, buck_duty, absPIDoutput;
	assert_param(dcdc != NULL);
	// 环路未开启
	if (dcdc->mode == Wait_Mode)
	{
		*dcdc->config.boost_duty = dcdc->config.min_duty;
		*dcdc->config.buck_duty = dcdc->config.min_duty;
		return;
	}
	if (dcdc->count++ >= 100)
	{
		// 缓启动
		dcdc->count = 0;
		dcdc->expect_Vout = Ramp_float(dcdc->Set_Vout, dcdc->expect_Vout, dcdc->config.V_ramp_ratio);
		// 钳位最大电压
		if (dcdc->config.Vin != NULL)
			dcdc->Vloop_max = *dcdc->config.Vin > 0 ? dcdc->Set_Vout / *dcdc->config.Vin * 1.1f : dcdc->config.max_transform_ratio;
		else
		dcdc->Vloop_max = dcdc->config.max_transform_ratio;
		dcdc->Vloop_max = min(dcdc->config.max_transform_ratio, dcdc->Vloop_max);
		// 参数合法性检查
		if (IS_NAN_INF(dcdc->config.Vloop_PID.u_1) || IS_NAN_INF(dcdc->config.Iloop_PID.u_1))
		{
			// 出现nan或者inf进行断言
			*dcdc->config.boost_duty = dcdc->config.min_duty;
			*dcdc->config.buck_duty = dcdc->config.min_duty;
			assert_param(0);
			// 断言关闭的状态则复位,关闭控制器
			dcdc->config.Vloop_PID.u_1 = 0;
			dcdc->config.Iloop_PID.u_1 = 0;
			dcdc->mode = Wait_Mode;
		}

	}
	// 反馈双环控制
	Vloop_out = Incremental_PID(&dcdc->config.Vloop_PID, dcdc->expect_Vout - *dcdc->config.Vout, dcdc->Vloop_max, 0);
	if (dcdc->config.flag.reverse_current == ENABLE)
		Iloop_out = Incremental_PID(&dcdc->config.Iloop_PID, dcdc->Set_Iout - *dcdc->config.Iout,
																dcdc->config.max_transform_ratio, -dcdc->config.max_transform_ratio);
	else
		Iloop_out = Incremental_PID(&dcdc->config.Iloop_PID, dcdc->Set_Iout - *dcdc->config.Iout, dcdc->config.max_transform_ratio, 0);
	// 双环竞争
	if (Iloop_out < Vloop_out)
	{
		// 钳位住电压环,避免电压环饱和
		Vloop_out = Iloop_out;
		dcdc->config.Vloop_PID.u_1 = Vloop_out;
		dcdc->status = CC_Status;
		PIDoutput = Iloop_out;
	}	
	else
	{
		dcdc->status = CV_Status;
		PIDoutput = Vloop_out;
	}
	// 变换比转换为占空比
	absPIDoutput = abs(PIDoutput);
	//[0,0.8]:buck
	if (absPIDoutput <= 0.8f)
	{
		dcdc->mode = Buck_Mode;
		boost_duty = dcdc->config.max_duty;
		buck_duty = max(absPIDoutput * dcdc->config.max_duty, dcdc->config.min_duty);
	}
	//(0.8,1.25]:buck-boost
	else if (absPIDoutput <= 1.25f)
	{
		dcdc->mode = Buck_Boost_Mode;
		boost_duty = max((0.444f + 0.444f / absPIDoutput) * dcdc->config.max_duty, dcdc->config.min_duty);
		buck_duty = max(0.444f * (1 + absPIDoutput) * dcdc->config.max_duty, dcdc->config.min_duty);
	}
	//(1.25,+inf):boost
	else
	{
		dcdc->mode = Boost_Mode;
		boost_duty = max(dcdc->config.max_duty / absPIDoutput, dcdc->config.min_duty);
		buck_duty = dcdc->config.max_duty;
	}

	// 占空比写入定时器
	if (PIDoutput >= 0)
	{
		*dcdc->config.boost_duty = boost_duty;
		*dcdc->config.buck_duty = buck_duty;
	}
	else
	{
		*dcdc->config.boost_duty = buck_duty;
		*dcdc->config.buck_duty = boost_duty;
	}
}

/**
 * @brief 最大功率点追踪初始化
 * @param[out] mppt_config 最大功率点配置结构体
 * @param[in] max_duty 最大占空比
 * @param[in] min_duty 最小占空比
 * @param[in] step_longth 每次迭代步长
 */
void mppt_Init(mppt_config_t *mppt_config, float max_duty, float min_duty, float step_longth)
{
	assert_param(mppt_config != NULL);
	assert_param(max_duty <= 1 && max_duty >= 0);
	assert_param(min_duty <= max_duty && min_duty >= 0);
	assert_param(step_longth < 1 && step_longth > 0);

	mppt_config->max_duty = max_duty;
	mppt_config->min_duty = min_duty;
	mppt_config->step_longth = step_longth;

	mppt_config->Pin = 0;
	mppt_config->lest_Pin = 0;
	mppt_config->mppt_duty = max_duty;
	mppt_config->mppt_direction = mppt_sub;
}

/**
 * @brief 最大功率点追踪
 * @param[in,out] mppt_config 最大功率点配置结构体
 * @param[in] Vin 输入电压
 * @param[in] Iin 输入电流
 * @return mppt占空比(上管占空比)
 */
float mppt(mppt_config_t *mppt_config, float Vin, float Iin)
{
	assert_param(mppt_config != NULL);

	mppt_config->lest_Pin = mppt_config->Pin;
	mppt_config->Pin = Vin * Iin;

	if (abs(mppt_config->Pin - mppt_config->lest_Pin) < 0.05f)
	{
		// 上一次功率误差和这一次误差小于则不操作，认为已经达到最大功率点。
	}
	else if (mppt_config->Pin > mppt_config->lest_Pin)
	{
		// 这一次功率误差小于上一次误差，往当前方向移动
		if (mppt_config->mppt_direction == mppt_add)
			mppt_config->mppt_duty += mppt_config->step_longth;
		else
			mppt_config->mppt_duty -= mppt_config->step_longth;
	}
	else
	{
		// 这一次功率误差大于上一次误差，往反方向移动
		if (mppt_config->mppt_direction == mppt_add)
		{
			mppt_config->mppt_duty -= mppt_config->step_longth;
			mppt_config->mppt_direction = mppt_sub;
		}
		else
		{
			mppt_config->mppt_duty += mppt_config->step_longth;
			mppt_config->mppt_direction = mppt_add;
		}
	}

	mppt_config->mppt_duty = constrain(mppt_config->mppt_duty, mppt_config->max_duty, mppt_config->min_duty);
	return mppt_config->mppt_duty;
}

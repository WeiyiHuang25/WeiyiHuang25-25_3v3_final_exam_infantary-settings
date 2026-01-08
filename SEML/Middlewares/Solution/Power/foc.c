#include "foc.h"
/**
 * @brief clarke变换 将ABC坐标系转换成αβ坐标系
 * @param ABC_frame ABC坐标系的坐标值
 * @param alpha_beta_frame αβ坐标系的坐标值指针
 * @attention 该变换为等幅值变换，等向量需乘以3/2，等功率需要乘sqrt(27/8)
 */
void clarke_transform(const ABC_frame_t ABC_frame, alpha_beta_frame_t *alpha_beta_frame)
{
	alpha_beta_frame->alpha = ABC_frame.A;
	alpha_beta_frame->beta = 1.0f / SQRT_3 * ABC_frame.A + 2.0f / SQRT_3 * ABC_frame.B;
}

/**
 * @brief clarke逆变换 将αβ坐标系转换成ABC坐标系
 * @param[in] alpha_beta_frame αβ坐标系的坐标值
 * @param[out] ABC_frame ABC坐标系的坐标值指针
 * @attention 该变换为等幅值变换，等向量需乘以2/3，等功率需要乘sqrt(8/27)
 */
void inv_clarke_transform(alpha_beta_frame_t alpha_beta_frame, ABC_frame_t *ABC_frame)
{
	ABC_frame->A = alpha_beta_frame.alpha;
	ABC_frame->B = -1.0f / 2.0f * alpha_beta_frame.alpha + SQRT_3 / 2.0f * alpha_beta_frame.beta;
	ABC_frame->C = -1.0f / 2.0f * alpha_beta_frame.alpha - SQRT_3 / 2.0f * alpha_beta_frame.beta;
}

/**
 * @brief park变换 将αβ坐标系转换成dq坐标系
 * @param[in] alpha_beta_frame αβ坐标系的坐标值
 * @param[in] wt 当前转子磁通位置
 * @param[out] dq_frame dq坐标系的坐标值指针
 */
void park_transform(alpha_beta_frame_t alpha_beta_frame, float wt, dq_frame_t *dq_frame)
{
	float coswt, sinwt;
	math_sin_cos(wt, &sinwt, &coswt);
	dq_frame->d = coswt * alpha_beta_frame.alpha + sinwt * alpha_beta_frame.beta;
	dq_frame->q = -sinwt * alpha_beta_frame.alpha + coswt * alpha_beta_frame.beta;
}

/**
 * @brief park逆变换 将αβ坐标系转换成dq坐标系
 * @param[in] dq_frame dq坐标系的坐标值
 * @param[in] wt 当前转子磁通位置
 * @param[out] alpha_beta_frame αβ坐标系的坐标值指针
 */
void inv_park_transform(dq_frame_t dq_frame, float wt, alpha_beta_frame_t *alpha_beta_frame)
{
	float coswt, sinwt;
	math_sin_cos(wt, &sinwt, &coswt);
	alpha_beta_frame->alpha = coswt * dq_frame.d - sinwt * dq_frame.q;
	alpha_beta_frame->beta = sinwt * dq_frame.d + coswt * dq_frame.q;
}

/**
 * @brief 矢量扇区判断
 * @param[in] U_alpha_beta Uαβ
 * @return N值，与所在扇区一一对应
 * @retval 与所在扇区映射关系为(前N后扇区)(3,1)(1,2)(5,3)(4,4)(6,5)(2,6)
 */
uint8_t vector_sector_judgment(alpha_beta_frame_t U_alpha_beta)
{
	uint8_t temp = 0x00;
	temp |= (U_alpha_beta.beta >= 0) ? 0x01 : 0x00;
	temp |= (SQRT_3 * U_alpha_beta.alpha - U_alpha_beta.beta >= 0) ? 0x02 : 0x00;
	temp |= (-SQRT_3 * U_alpha_beta.alpha - U_alpha_beta.beta >= 0) ? 0x04 : 0x00;
	return temp;
}

/**
 * @brief 计算矢量作用时间
 * @param[in] U_alpha_beta Uαβ
 * @param[in] N N值
 * @param[in] Udc 直流电压
 * @param[out] T 非零矢量作用时间数组
 */
void calculate_vector_action_time(alpha_beta_frame_t U_alpha_beta, uint8_t N, float Udc, float T[2])
{
	float temp = 0.5f / Udc;
	static ABC_frame_t Uref;
	// 参考电压矢量计算
	Uref.A = 2.0f * temp * U_alpha_beta.beta;
	Uref.B = temp * (SQRT_3 * U_alpha_beta.alpha + U_alpha_beta.beta);
	Uref.C = temp * (-SQRT_3 * U_alpha_beta.alpha + U_alpha_beta.beta);
	// 映射到各个扇区的基本电压矢量作用时间
	switch (N)
	{
	case 1:
		T[0] = Uref.C;
		T[1] = Uref.B;
		break;
	case 2:
		T[0] = Uref.B;
		T[1] = -Uref.A;
		break;
	case 3:
		T[0] = -Uref.C;
		T[1] = Uref.A;
		break;
	case 4:
		T[0] = -Uref.A;
		T[1] = Uref.C;
		break;
	case 5:
		T[0] = Uref.A;
		T[1] = -Uref.B;
		break;
	case 6:
		T[0] = -Uref.B;
		T[1] = -Uref.C;
		break;
	}
	// 过调制处理
	temp = T[0] + T[1];
	if (1.0f - temp < 0)
	{
		temp = 1.0f / temp;
		T[0] *= temp;
		T[1] *= temp;
	}
}

/**
 * @brief 扇区矢量作用时间切换
 * @param[in] T 非零矢量作用时间数组
 * @param[in] N N值
 * @param[out] Tcm 调制波数组，范围[0,1]
 */
void sector_vector_switching(float T[2], uint8_t N, ABC_frame_t *Tcm)
{
	ABC_frame_t Tref;
	Tref.A = (1 - T[0] - T[1]) * 0.5f;
	Tref.B = Tref.A + T[0];
	Tref.C = Tref.B + T[1];
	switch (N)
	{
	case 1:
		Tcm->A = Tref.B;
		Tcm->B = Tref.A;
		Tcm->C = Tref.C;
		break;
	case 2:
		Tcm->A = Tref.A;
		Tcm->B = Tref.C;
		Tcm->C = Tref.B;
		break;
	case 3:
		Tcm->A = Tref.A;
		Tcm->B = Tref.B;
		Tcm->C = Tref.C;
		break;
	case 4:
		Tcm->A = Tref.C;
		Tcm->B = Tref.B;
		Tcm->C = Tref.A;
		break;
	case 5:
		Tcm->A = Tref.C;
		Tcm->B = Tref.A;
		Tcm->C = Tref.B;
		break;
	case 6:
		Tcm->A = Tref.B;
		Tcm->B = Tref.C;
		Tcm->C = Tref.A;
		break;
	default:
		Tcm->A = 0;
		Tcm->B = 0;
		Tcm->C = 0;
	}
}

/**
 * @brief 空间矢量调制算法
 * @param[in] U_alpha_beta Uα,Uβ电压值
 * @param[in] Udc 母线电压
 * @param[out] Tcm 三相占空比指针
 */
void svpwm(alpha_beta_frame_t U_alpha_beta, float Udc, ABC_frame_t *Tcm)
{
	uint8_t N;
	float T[2];
	N = vector_sector_judgment(U_alpha_beta);
	calculate_vector_action_time(U_alpha_beta, N, Udc, T);
	sector_vector_switching(T, N, Tcm);
}

/**
 * @brief 三相正弦调制算法
 * @param[in] U_alpha_beta Uα,Uβ电压值
 * @param[in] Udc 母线电压
 * @param[out] Tcm 三相占空比指针
 */
void Three_Phase_spwm(alpha_beta_frame_t U_alpha_beta, float Udc, ABC_frame_t *Tcm)
{
	float temp = 1 / (Udc * 2);
	inv_clarke_transform(U_alpha_beta, Tcm);
	Tcm->A = Tcm->A * temp + 0.5f;
	Tcm->B = Tcm->B * temp + 0.5f;
	Tcm->C = Tcm->C * temp + 0.5f;
}

/**
 * @brief 单相正弦调制算法
 * @param[in] n 幅值
 * @param[in] wt 当前转子磁通位置
 * @param[out] Tcm 三相占空比指针
 */
void Single_Phase_spwm(float n, float wt, ABC_frame_t *Tcm)
{
	float temp = n * 0.5f * math_sin(wt) + 0.5f;
	Tcm->A = temp;
	Tcm->B = 1 - temp;
	Tcm->C = 0;
}

/**
 * @brief 角度归一化
 * @param angle 输入弧度
 * @return 约束到[0,2PI)的结果
 */
float Normalize_Angle(float angle)
{
	float a = math_fmod(angle, 2 * PI);
	return a >= 0 ? a : (a + 2 * PI);
}

/**
 * @brief FOC闭环控制环路
 * @param foc foc句柄
 * @param I_abc abc相电流
 */
void Motor_FOC_Loop(Motor_FOC_Control_t *foc)
{
	alpha_beta_frame_t I_ab, U_ab;
	ABC_frame_t I_abc = {
			.A = *foc->Ia,
			.B = *foc->Ib};
	float timer_ref;
	// 将电流解耦 转换到dq轴
	clarke_transform(I_abc, &I_ab);
	park_transform(I_ab, *foc->electrical_angle + foc->driver.zero_i_angle, &foc->i_dq);
	// 电流环
	if (foc->status.OpenLoop == 0)
	{
		foc->u_dq.d = Basic_PI_Controller(&foc->PID_id, 0, foc->i_dq.d);
		foc->u_dq.d += - foc->motor.speed * foc->param.d_phase_inductance * foc->i_dq.q;
		foc->u_dq.q = Basic_PI_Controller(&foc->PID_iq, foc->expect_iq, foc->i_dq.q);
		foc->u_dq.q += foc->motor.speed * foc->param.q_phase_inductance * foc->i_dq.d;
	}
	// svpwm
	inv_park_transform(foc->u_dq, *foc->electrical_angle, &U_ab);
	svpwm(U_ab, 24, &foc->Tcm);
	timer_ref = foc->driver.timer_ref;
	// 输出到定时器
	*foc->driver.timer_phase.A = timer_ref * foc->Tcm.A;
	*foc->driver.timer_phase.B = timer_ref * foc->Tcm.B;
	*foc->driver.timer_phase.C = timer_ref * foc->Tcm.C;
}
static void Inc_Position(float *position, float new_position)
{
	float temp;
	temp = new_position - *position;
	if (abs(temp) > PI) // 转过半圈
	{
		if (temp < 0)
			*position += 2 * PI + temp;
	}
	else
		*position += temp;
}

/**
 * @brief 标定foc位置传感器和极对数
 * @param foc foc句柄
 * @param current_sample 电流传感器安装位置
 */
SEML_StatusTypeDef Motor_Calibration_Sensor(Motor_FOC_Control_t *foc, Motor_FOC_Current_Sample_t current_sample)
{
	uint32_t *timer_phase[3], start_tick;
	float angle = 0, start_position, zero_electric_angle, temp;
	float *ptemp;
	timer_phase[0] = foc->driver.timer_phase.A;
	timer_phase[1] = foc->driver.timer_phase.B;
	timer_phase[2] = foc->driver.timer_phase.C;
	foc->status.OpenLoop = 1;
	// 给予d轴一个小电压,
	foc->u_dq.d = 5;
	foc->u_dq.q = 0;
	foc->driver.zero_electric_angle = 0;
	foc->electrical_angle = &angle;
	// 等待电机锁定在d轴
	SEML_Delay(500);
	// 记录此时电角度为绝对零电角
	zero_electric_angle = foc->driver.electrical_angle;
	start_position = foc->motor.inc_position;
	// 让电机旋转一个机械角度
	start_tick = SEML_GetTick();
	while (abs(foc->motor.inc_position - start_position) < 2 * PI)
	{
		angle = (SEML_GetTick() - start_tick) * 0.001f * 10 * PI;
		if (SEML_GetTick() - start_tick > 1000)
			return SEML_TIMEOUT;
	}
	// 继续旋转一个电角度
	temp = (int32_t)((angle + PI / 6) / (2 * PI));
	start_tick = SEML_GetTick();
	angle = 0;
	start_position = foc->motor.inc_position;
	while (angle < 2 * PI)
		angle = (SEML_GetTick() - start_tick) * 0.001f * 10 * PI;
	// 两次辨识结果不相等认为标定失败
	if ((int32_t)(2.05f * PI / abs(foc->motor.inc_position - start_position)) != temp)
		return SEML_TIMEOUT;
	foc->param.pole_pairs = temp;
	// 读取速度是否和角度相反，若相反交换相序
	if (foc->motor.speed < 0)
	{
		foc->driver.timer_phase.A = timer_phase[0];
		foc->driver.timer_phase.B = timer_phase[2];
		foc->driver.timer_phase.C = timer_phase[1];
		// 交换电流采样,矫正电流相位
		switch (current_sample)
		{
		case FOC_Current_Sample_AB:
			break;
		case FOC_Current_Sample_AC:
			foc->driver.zero_i_angle = 0 * PI / 3;
			break;
		case FOC_Current_Sample_BC:
			break;
		default:
			assert_param(0);
		}
		ptemp = foc->Ia;
		foc->Ia = foc->Ib;
		foc->Ib = ptemp;
	}
	else
	{
		// 矫正电流相位
		switch (current_sample)
		{
		case FOC_Current_Sample_AB:
			break;
		case FOC_Current_Sample_AC:
			foc->driver.zero_i_angle = 2 * PI / 3;
			break;
		case FOC_Current_Sample_BC:
			break;
		}
	}
	foc->driver.zero_electric_angle = zero_electric_angle;
	foc->electrical_angle = &foc->driver.electrical_angle;
	foc->status.OpenLoop = 0;
	return SEML_OK;
}

/**
 * @brief 电机参数辨识
 * @param foc foc句柄
 */
SEML_StatusTypeDef Motor_Parameter_Identification(Motor_FOC_Control_t *foc)
{

}
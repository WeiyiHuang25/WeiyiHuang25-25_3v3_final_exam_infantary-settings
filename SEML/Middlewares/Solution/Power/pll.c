#include "pll.h"
/**
 * @brief 同步坐标系锁相环初始化
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] Vm 交流电压幅值(V)
 * @param[in] Ts 调节时间
*/
void SRF_PLL_Init(SRF_PLL_Config_t *SRF_PLL_Config,const float Vm,const float Ts)
{
    float Wc;
    assert_param(SRF_PLL_Config != NULL);
    assert_param(Ts > 0);
    assert_param(Vm > 0);

    // SRF-PLL为二阶系统，可以用二阶系统的动态响应来估计其PI值
    // 令ζ=0.707，调节自然频率

    Wc = 3.5f / (Ts * 0.707);
    SRF_PLL_Config->LF_P = 2.0f * 0.707f * Wc / Vm;
    SRF_PLL_Config->LF_I = Wc * Wc / Vm;
    SRF_PLL_Config->LF_integral = 0;
    SRF_PLL_Config->theta_hat = 0;
}

/**
 * @brief 同步坐标系锁相环参数配置
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] LF_P 低通滤波器比例系数
 * @param[in] LF_I 低通滤波器积分系数
*/
void SRF_PLL_Config(SRF_PLL_Config_t *SRF_PLL_Config,const float LF_P,const float LF_I)
{
    
    assert_param(SRF_PLL_Config != NULL);

    SRF_PLL_Config->LF_P = LF_P;
    SRF_PLL_Config->LF_I = LF_I;
    SRF_PLL_Config->LF_integral = 0;
    SRF_PLL_Config->theta_hat = 0;
}

/**
 * @brief 同步坐标系锁相环
 * SRF_PLL适用于无直流分量的对称三相电压的锁相.
 * @param[in,out] SRF_PLL_Config SRF_PLL配置结构体
 * @param[in] Uab α-β轴电压(V)
 * @param[in] sample_time 采样时间(s)
 * @return 预估相位(rad)
 * @note 如需要读取Udq和ω请直接在配置结构体里面读取
*/
float SRF_PLL(SRF_PLL_Config_t *SRF_PLL_Config,alpha_beta_frame_t Uab,float sample_time)
{
    float temp;
    
    assert_param(SRF_PLL_Config != NULL);

    // 鉴相器 PD
    park_transform(Uab,SRF_PLL_Config->theta_hat,&SRF_PLL_Config->Udq);
    // 低通滤波器 LF
    SRF_PLL_Config->LF_integral += SRF_PLL_Config->Udq.q * SRF_PLL_Config->LF_I * sample_time;
    SRF_PLL_Config->w_hat = SRF_PLL_Config->LF_P * SRF_PLL_Config->Udq.q + SRF_PLL_Config->LF_integral;
    // 压控振荡器 VCO
    temp = SRF_PLL_Config->theta_hat;
    SRF_PLL_Config->theta_hat += SRF_PLL_Config->w_hat * sample_time;
    SRF_PLL_Config->theta_hat = math_fmod(SRF_PLL_Config->theta_hat,2*PI);
    return SRF_PLL_Config->theta_hat;
}

/**
 * @brief 二阶广义正交信号发生器初始化
 * @param[out] SOGI_QSG_Config 二阶广义正交信号发生器配置结构体指针
 * @param[in] K 阻尼系数
 */
void SOGI_QSG_Init(SOGI_QSG_Config_t *SOGI_QSG_Config,float K)
{
    SOGI_QSG_Config->integral[0] = 0;
    SOGI_QSG_Config->integral[1] = 0;
    SOGI_QSG_Config->K = K;
}

/**
 * @brief 二阶广义正交信号发生器
 * @param[in,out] SOGI_QSG_Config 二阶广义正交信号发生器配置结构体
 * @param[out] Uab 输出的αβ轴正交电压(V)
 * @param[in] Vin 输入电压(V)
 * @param[in] w SOGI谐振角速度(输入电压角速度)(rad/s)
 * @param[in] sample_time 采样时间(s)
 */
void SOGI_QSG(SOGI_QSG_Config_t *SOGI_QSG_Config,alpha_beta_frame_t *Uab,float Vin,float w, float sample_time)
{
    float temp = sample_time * w;
    SOGI_QSG_Config->integral[0] += temp * (SOGI_QSG_Config->K * (Vin - SOGI_QSG_Config->integral[0]) - SOGI_QSG_Config->integral[1]);
    SOGI_QSG_Config->integral[1] += SOGI_QSG_Config->integral[0] * temp;
    Uab->alpha = SOGI_QSG_Config->integral[0];
    Uab->beta = SOGI_QSG_Config->integral[1];
}

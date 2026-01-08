
#include "AHT20.h"

/**
 * @brief  读AHT20 设备状态字
 * @param aht20 aht20句柄
 * @retval uint8_t 设备状态字
 */
static uint8_t AHT20_ReadStatusCmd(AHT20_t *aht20)
{
	uint8_t tmp[1];
	SEML_I2C_Mem_Read(&aht20->i2c_handle, ATH20_SLAVE_ADDRESS, AHT20_STATUS_REG, I2C_MEMADD_SIZE_8BIT, tmp, 1, AHT20_TIMEOUT);
	return tmp[0];
}

/**
 * @brief  读AHT20 设备状态字 中的Bit3: 校准使能位
 * @param aht20 aht20句柄
 * @retval uint8_t 校准使能位：1 - 已校准; 0 - 未校准
 */
static uint8_t AHT20_ReadCalEnableCmd(AHT20_t *aht20)
{
	uint8_t tmp;
	tmp = AHT20_ReadStatusCmd(aht20);
	return (tmp >> 3) & 0x01;
}

/**
 * @brief  读AHT20 设备状态字 中的Bit7: 忙标志
 * @param aht20 aht20句柄
 * @retval uint8_t 忙标志：1 - 设备忙; 0 - 设备空闲
 */
static uint8_t AHT20_ReadBusyCmd(AHT20_t *aht20)
{
	uint8_t tmp;
	tmp = AHT20_ReadStatusCmd(aht20);
	return (tmp >> 7) & 0x01;
}

/**
 * @brief  AHT20 芯片初始化命令
 * @param aht20 aht20句柄
 * @retval void
 */
static void AHT20_IcInitCmd(AHT20_t *aht20)
{
	uint8_t tmp[2];
	tmp[0] = 0x08;
	tmp[1] = 0x00;
	SEML_I2C_Mem_Write(&aht20->i2c_handle, ATH20_SLAVE_ADDRESS, AHT20_INIT_REG, I2C_MEMADD_SIZE_8BIT, tmp, 2, AHT20_TIMEOUT);
}

/**
 * @brief  AHT20 触发测量命令
 * @param aht20 aht20句柄
 * @retval void
 */
static void AHT20_TrigMeasureCmd(AHT20_t *aht20)
{
	uint8_t tmp[2];
	tmp[0] = 0x33;
	tmp[1] = 0x00;
	SEML_I2C_Mem_Write(&aht20->i2c_handle, ATH20_SLAVE_ADDRESS, AHT20_TrigMeasure_REG, I2C_MEMADD_SIZE_8BIT, tmp, 2, AHT20_TIMEOUT);
}

/**
 * @brief  AHT20 软复位命令
 * @param  void
 * @retval void
 */
static void AHT20_SoftResetCmd(AHT20_t *aht20)
{
	uint8_t tmp[1];
	SEML_I2C_Mem_Write(&aht20->i2c_handle, ATH20_SLAVE_ADDRESS, AHT20_SoftReset, I2C_MEMADD_SIZE_8BIT, tmp, 0, AHT20_TIMEOUT);
}

/**
 * @brief  AHT20 设备初始化
 * @param aht20 aht20句柄
 * @retval uint8_t：0 - 初始化AHT20设备成功; 1 - 初始化AHT20失败，可能设备不存在或器件已损坏
 */
SEML_StatusTypeDef AHT20_Init(AHT20_t *aht20)
{
	uint8_t rcnt = 2 + 1; // 软复位命令 重试次数，2次
	uint8_t icnt = 2 + 1; // 初始化命令 重试次数，2次

	while (--rcnt)
	{
		icnt = 2 + 1;

		SEML_Delay(40); // 上电后要等待40ms
		// 读取温湿度之前，首先检查[校准使能位]是否为1
		while ((!AHT20_ReadCalEnableCmd(aht20)) && (--icnt)) // 2次重试机会
		{
			SEML_Delay(10);
			// 如果不为1，要发送初始化命令
			AHT20_IcInitCmd(aht20);
			SEML_Delay(200); // 这个时间不确定，手册没讲
		}

		if (icnt) //[校准使能位]为1,校准正常
		{
			break; // 退出rcnt循环
		}
		else //[校准使能位]为0,校准错误
		{
			AHT20_SoftResetCmd(aht20); // 软复位AHT20器件，重试
			SEML_Delay(200);					 // 这个时间不确定，手册没讲
		}
	}

	if (rcnt)
	{
		SEML_Delay(200); // 这个时间不确定，手册没讲
		return SEML_OK;	 // AHT20设备初始化正常
	}
	return SEML_ERROR; // AHT20设备初始化失败
}

/**
 * @brief  AHT20 设备读取 相对湿度和温度（原始数据20Bit）
 * @param aht20 aht20句柄
 * @retval uint8_t：0-读取数据正常; 1-读取设备失败，设备一直处于忙状态，不能获取数据
 */
SEML_StatusTypeDef AHT20_ReadHT(AHT20_t *aht20)
{
	uint8_t cnt = 3 + 1; // 忙标志 重试次数，3次
	uint8_t tmp[6];
	uint32_t RetuData = 0;

	// 发送触发测量命令
	AHT20_TrigMeasureCmd(aht20);

	do
	{
		SEML_Delay(75);															 // 等待75ms待测量完成，忙标志Bit7为0
	} while (AHT20_ReadBusyCmd(aht20) && (--cnt)); // 重试3次

	if (cnt) // 设备闲，可以读温湿度数据
	{
		SEML_Delay(5);
		// 读温湿度数据
		SEML_I2C_Mem_Read(&aht20->i2c_handle, ATH20_SLAVE_ADDRESS, AHT20_STATUS_REG, I2C_MEMADD_SIZE_8BIT, tmp, 6, AHT20_TIMEOUT);
		// 计算相对湿度RH。原始值，未计算为标准单位%。
		RetuData = 0;
		RetuData = (RetuData | tmp[1]) << 8;
		RetuData = (RetuData | tmp[2]) << 8;
		RetuData = (RetuData | tmp[3]);
		RetuData = RetuData >> 4;
		aht20->HT[0] = RetuData;

		// 计算温度T。原始值，未计算为标准单位°C。
		RetuData = 0;
		RetuData = (RetuData | tmp[3]) << 8;
		RetuData = (RetuData | tmp[4]) << 8;
		RetuData = (RetuData | tmp[5]);
		RetuData = RetuData & 0xfffff;
		aht20->HT[1] = RetuData;

		return SEML_OK;
	}
	// 设备忙，返回读取失败
	return SEML_BUSY;
}

/**
 * @brief  AHT20 温湿度信号转换（由20Bit原始数据，转换为标准单位RH=%，T=°C）
 * @param  struct m_AHT20* aht：存储AHT20传感器信息的结构体
 * @retval uint8_t：0-计算数据正常; 1-计算数据失败，计算值超出元件手册规格范围
 */
SEML_StatusTypeDef AHT20_StandardUnitCon(AHT20_t *aht20)
{
	aht20->RH = (double)aht20->HT[0] * 100 / 1048576; // 2^20=1048576 //原式：(double)aht20->HT[0] / 1048576 *100，为了浮点精度改为现在的
	aht20->Temp = (double)aht20->HT[1] * 200 / 1048576 - 50;

	// 限幅,RH=0~100%; Temp=-40~85°C
	if ((aht20->RH >= 0) && (aht20->RH <= 100) && (aht20->Temp >= -40) && (aht20->Temp <= 85))
	{
		aht20->flag = SEML_OK;
		return 0; // 测量数据正常
	}
	else
	{
		aht20->flag = SEML_ERROR;
		return 1; // 测量数据超出范围，错误
	}
}
/**
 * @brief AHT20读取温湿度数据
 * @param aht20 aht20句柄
 * @return uint8_t
 */
SEML_StatusTypeDef AHT20_Read(AHT20_t *aht20)
{
	SEML_StatusTypeDef status = SEML_OK;
	status |= AHT20_ReadHT(aht20);
	if (status != SEML_OK)
		return status;
	status |= AHT20_StandardUnitCon(aht20);
	return status;
}
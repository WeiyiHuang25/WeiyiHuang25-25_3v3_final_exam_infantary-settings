/**
 ****************************(C) COPYRIGHT 2024 Polarbear****************************
* @file       CAN_cmd_SupCap.c/h
* @brief      CAN发送函数，通过CAN信号控制超级电容
* @history
*  Version    Date            Author          Modification
*  V1.0.0     Nov-29-2024     Penguin         1. 完成。
*
@verbatim
==============================================================================

==============================================================================
@endverbatim
****************************(C) COPYRIGHT 2024 Polarbear****************************
*/

#include "SuperCap.h"

extern Robo_Middleware_t robo_middleware;
void* sup_cap_measure_ptr = &SUP_CAP_MEASURE;

// CanCtrlData_s CAN_CTRL_DATA = {
// 	.tx_header.IDE = CAN_ID_STD,
// 	.tx_header.RTR = CAN_RTR_DATA,
// 	.tx_header.DLC = 2,
// };

/**
 * @brief          通过CAN发送超电板控制指令
 * @param[in]      can 发送数据使用的can口(1/2)
 * @param[in]      power 超电控制功率
 * @return         none
 */
void CanCmdSupCap(int16_t power)
{
	// hcan_t * hcan = NULL;
	// if (can == 1)
	// 	hcan = &hcan1;
	// else if (can == 2)
	// 	hcan = &hcan2;
	// if (hcan == NULL) return;

	// CAN_CTRL_DATA.hcan = hcan;

	// CAN_CTRL_DATA.tx_header.StdId = 0x210;

	// CAN_CTRL_DATA.tx_data[0] = (power >> 8);
	// CAN_CTRL_DATA.tx_data[1] = power;

	// CAN_SendTxMessage(&CAN_CTRL_DATA);

	uint8_t aData[2];
	aData[0] = (power >> 8);
	aData[1] = power;
	SEML_CAN_Send(&can1, 0x210, aData, 2, CAN_RTR_DATA);
}

/**
 * @brief       超级电容初始化
 * @param[in]   p_sup_cap 超级电容结构体
 * @param[in]   can 超级电容使用的can口
 */
void SupCapInit(SupCap_s * p_sup_cap, uint8_t can)
{
    p_sup_cap->id = 1;
    p_sup_cap->type = SUP_CAP_1;
    p_sup_cap->can = can;
    p_sup_cap->offline = true;

    memset(&p_sup_cap->fdb, 0, sizeof(p_sup_cap->fdb));
    memset(&p_sup_cap->set, 0, sizeof(p_sup_cap->set));
	SEML_CAN_Init(&can1, &hcan1, robo_middleware.can1_rx_buffer,10);
}

/**
 * @brief        SupCapFdbData: 获取超级电容反馈数据函数
 * @param[out]   dm_measure 电机数据缓存
 * @param[in]    rx_data 指向包含反馈数据的数组指针
 * @note         从接收到的数据中提取LK电机的反馈信息
 */
void SupCapFdbData(SupCapMeasure_s * sup_cap_measure, uint8_t * rx_data)
{
    sup_cap_measure->voltage_in = (uint16_t)(rx_data[1] << 8 | rx_data[0]);
    sup_cap_measure->voltage_cap = (uint16_t)(rx_data[3] << 8 | rx_data[2]);
    sup_cap_measure->current_in = (uint16_t)(rx_data[5] << 8 | rx_data[4]);
    sup_cap_measure->power_target = (uint16_t)(rx_data[7] << 8 | rx_data[6]);

    sup_cap_measure->last_fdb_time = HAL_GetTick();
}

/**
 * @brief          获取超级电容反馈数据
 * @param[out]     p_sup_cap 超级电容结构体 
 * @note           测试期间临时使用，后续将会删除，正式版中使用GetSupCapMeasure
 * @return         none
 */
void GetSupCapFdbData(SupCapMeasure_s * p_sup_cap)
{
    memcpy(p_sup_cap, &SUP_CAP_MEASURE, sizeof(SupCapMeasure_s));
}

/**
 * @brief          获取超级电容反馈数据
 * @param[out]     p_sup_cap 超级电容结构体 
 * @return         none
 */
void GetSupCapMeasure(SupCap_s * p_sup_cap)
{
    p_sup_cap->fdb.voltage_in = SUP_CAP_MEASURE.voltage_in / 100.0f;
    p_sup_cap->fdb.voltage_cap = SUP_CAP_MEASURE.voltage_cap / 100.0f;
    p_sup_cap->fdb.current_in = SUP_CAP_MEASURE.current_in / 50.0f;
    p_sup_cap->fdb.power_target = SUP_CAP_MEASURE.power_target;
}
/************************ END OF FILE ************************/

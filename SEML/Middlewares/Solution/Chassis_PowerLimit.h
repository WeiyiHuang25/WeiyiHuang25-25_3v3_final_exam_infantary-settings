#ifndef _CHASSIS_POWERLIMIT_H__
#define _CHASSIS_POWERLIMIT_H__
#include "SEML_common.h"
#include "reference.h"
#include "SuperCap.h"
#include "PID.h"
typedef struct
{
	Reference_t *reference;
	SuperCap_t *supercap;
	PIDConfig_t Power_PID;
	struct
	{
		uint8_t enable_supercap : 1;
	} status;

} Chassis_PowerLimit_t;

#define CHASSIS_DRIVE_IMAX 12000.0f
#define CHASSIS_MOTOR_COUNT 4

void Chassis_Power_Limit_Init(Chassis_PowerLimit_t *chassis_power_limit,	Reference_t *reference,SuperCap_t *supercap,float sample_time);

void Chassis_Power_Limit(Chassis_PowerLimit_t *chassis_power_limit,float send_data[CHASSIS_MOTOR_COUNT]);
#endif

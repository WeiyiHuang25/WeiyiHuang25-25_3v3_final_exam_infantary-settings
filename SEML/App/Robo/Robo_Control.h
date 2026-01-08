#ifndef _ROBO_CONTROL_H_
#define _ROBO_CONTROL_H_
#include "SEML_common.h"
#include "DR16_Remote.h"
#include "Robo_Common.h"
#include "Motor.h"
#include "Robo_USB.h"
#include "RC_Ctrl_Key.h"
#include "EzTuner.h"
typedef enum
{
  Scource_None = 0,
  Scource_Remote,
  Scource_Key,
  Scource_Upper_Computer,
} Control_Scource_t;
typedef struct 
{
	Control_Scource_t remote_source;
}Task_Control_t;

extern Keyboard_t keyboard;

void Robo_Control_Init(void *config);
void Robo_Control_Task(void *config);
#endif

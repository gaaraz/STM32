#ifndef __Inf_JOYSTICK_KEY_H__
#define __Inf_JOYSTICK_KEY_H__

#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "adc.h"
#include "Com_Config.h"
#include "Com_Debug.h"

void Inf_JoyStickAndKey_Init(void);

void Inf_JoyStickAndKey_JoyStickScan(void);

Com_Key Inf_JoyStickAndKey_KeyScan(void);

#endif
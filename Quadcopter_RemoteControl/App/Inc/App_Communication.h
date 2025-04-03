#ifndef __APP_COMMUNICATION_H__
#define __APP_COMMUNICATION_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Com_Debug.h"
#include "Com_Config.h"
#include "Inf_SI24R1.h"


void App_Communication_Start(void);
void App_Communication_SendJoyStickData(void);

#endif
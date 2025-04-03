#ifndef __APP_COMMUNICATION_H__
#define __APP_COMMUNICATION_H__

#include "FreeRTOS.h"
#include "task.h"

#include "Com_Debug.h"
#include "Com_Config.h"
#include "Inf_SI24R1.h"

void App_Communication_Start(void);
Com_Status App_Communication_ReceiveJoyStickData(void);

#endif
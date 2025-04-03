#ifndef __INF_MOTOR_H__
#define __INF_MOTOR_H__

#include "Com_Config.h"
#include "Com_Debug.h"

void Inf_Motor_Init(void);

void Inf_Motor_SetSpeed(MotorStruct *motor);

void Inf_Motor_AllMotorsWork(void);

#endif



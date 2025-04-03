#include "Inf_Motor.h"


void Inf_Motor_Init(void)
{
    /* 启动四个定时器 */
    HAL_TIM_PWM_Start(motorLeftTop.p_htim, motorLeftTop.channel);
    HAL_TIM_PWM_Start(motorLeftBottom.p_htim, motorLeftBottom.channel);
    HAL_TIM_PWM_Start(motorRightTop.p_htim, motorRightTop.channel);
    HAL_TIM_PWM_Start(motorRightBottom.p_htim, motorRightBottom.channel);

    /* 设置电机初始速度 */
    motorLeftTop.speed = 0;
    motorLeftBottom.speed = 0;
    motorRightTop.speed = 0;
    motorRightBottom.speed = 0;

    Inf_Motor_AllMotorsWork();
}


void Inf_Motor_SetSpeed(MotorStruct *motor)
{
    /* 直流电机限速 */
    motor->speed = LIMIT(motor->speed, 0, 1000);
    /* 设置电机速度 */
    __HAL_TIM_SET_COMPARE(motor->p_htim, motor->channel, motor->speed);
}

void Inf_Motor_AllMotorsWork(void)
{
    Inf_Motor_SetSpeed(&motorLeftTop);
    Inf_Motor_SetSpeed(&motorLeftBottom);
    Inf_Motor_SetSpeed(&motorRightTop);
    Inf_Motor_SetSpeed(&motorRightBottom);
}
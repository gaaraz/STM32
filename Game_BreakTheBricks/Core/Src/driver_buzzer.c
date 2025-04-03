#include "driver_buzzer.h"
#include "tim.h"

static TimerHandle_t g_TimerSound;


void TimerSoundFunc(void){
    Buzzer_Control(0);
}

void Buzzer_Init(void)
{
    // TIM_OC_InitTypeDef sConfig = { 0 };
    // GPIO_InitTypeDef GPIO_InitStruct = {0};

    // /**TIM1 GPIO Configuration
    // PA8     ------> TIM1_CH1
    // */
    // GPIO_InitStruct.Pin = GPIO_PIN_8;
    // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // //HAL_TIM_Base_DeInit(g_HPWM_PassiveBuzzer);
    // htim1.Instance = TIM1;
    // htim1.Init.Prescaler = 71;
    // htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    // htim1.Init.Period = 999;
    // htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // htim1.Init.RepetitionCounter = 0;
    // htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    // //HAL_TIM_Base_Init(g_HPWM_PassiveBuzzer);
    // TIM_Base_SetConfig(htim1.Instance, &htim1.Init);
    // //return;


    // sConfig.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    // sConfig.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    // sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    // sConfig.Pulse      = 499;                // 在 PWM1 模式下,50%占空比
    
    // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfig, TIM_CHANNEL_1);

    g_TimerSound = xTimerCreate("TimerSound", 200, pdFALSE, NULL, TimerSoundFunc);
}

void Buzzer_Control(int on){
    if(on){
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    }else{
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    }
}


void Buzzer_Set_Freq_Duty(int freq, int duty){
    TIM_OC_InitTypeDef sConfigOC = {0};

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    HAL_TIM_Base_DeInit(&htim1);
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 71;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000000 / freq - 1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim1);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = (1000000/freq-1) * duty/100;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void buzzer_buzz(int freq, int time_ms){
    Buzzer_Set_Freq_Duty(freq, 50);

    /* 启动定时器  */
    xTimerChangePeriod(g_TimerSound, time_ms, 0);
}


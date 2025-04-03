#include "Inf_IP5305T.h"

/* 短按：引脚拉低，延迟100ms，引脚拉高 */
static void Inf_IP5305T_Press(void){
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    
    vTaskDelay(100);

    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
}

/* 保持电源开 */
void Inf_IP5305T_Open(void){
    Inf_IP5305T_Press();
}

/* 1s内短按两次，关闭电源 */
void Inf_IP5305T_Close(void){
    Inf_IP5305T_Press();
    vTaskDelay(500);
    Inf_IP5305T_Press();
}


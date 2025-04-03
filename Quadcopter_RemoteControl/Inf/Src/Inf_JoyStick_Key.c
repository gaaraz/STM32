#include "Inf_JoyStick_Key.h"


#define READ_LEFT()         HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin)
#define READ_UP()           HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)
#define READ_RIGHT()        HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin)
#define READ_DOWN()         HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin)
#define READ_LEFT_TOP()     HAL_GPIO_ReadPin(KEY_LEFT_TOP_GPIO_Port, KEY_LEFT_TOP_Pin)
#define READ_RIGHT_TOP()    HAL_GPIO_ReadPin(KEY_RIGHT_TOP_GPIO_Port, KEY_RIGHT_TOP_Pin)

static uint16_t buff[4] = {0};

/* 摇杆初始化 */
void Inf_JoyStickAndKey_Init(void){
    // 打印调试信息
    debug_printf("JoyStickAndKey_Init Start");
    /* ADC校准 */
    HAL_ADCEx_Calibration_Start(&hadc1);
    /* 启动ADC转换 */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buff, 4);
}


/* 摇杆扫描 */
void Inf_JoyStickAndKey_JoyStickScan(void){
    joyStick.THR = buff[0];
    joyStick.YAW = buff[1];
    joyStick.PIT = buff[2];
    joyStick.ROL = buff[3];
}


/* 按键扫描 */
Com_Key Inf_JoyStickAndKey_KeyScan(void){
    if (READ_LEFT() == GPIO_PIN_RESET)
    {
        vTaskDelay(20);
        while (READ_LEFT() == GPIO_PIN_RESET);
        vTaskDelay(20);
        return COM_KEY_LEFT;
    }else if (READ_RIGHT() == GPIO_PIN_RESET){
        vTaskDelay(20);
        while (READ_RIGHT() == GPIO_PIN_RESET);
        vTaskDelay(20);
        return COM_KEY_RIGHT;
    }else if (READ_DOWN() == GPIO_PIN_RESET){
        vTaskDelay(20);
        while (READ_DOWN() == GPIO_PIN_RESET);
        vTaskDelay(20);
        return COM_KEY_DOWN;
    }else if (READ_UP() == GPIO_PIN_RESET){
        vTaskDelay(20);
        while (READ_UP() == GPIO_PIN_RESET);
        vTaskDelay(20);
        return COM_KEY_UP;
    }else if (READ_LEFT_TOP() == GPIO_PIN_RESET){
        uint16_t times = 0;
        vTaskDelay(20);
        while (READ_LEFT_TOP() == GPIO_PIN_RESET && times < 100){
            times++;
            vTaskDelay(10);
        }
        if (times == 100){
            vTaskDelay(20);
            while (READ_LEFT_TOP() == GPIO_PIN_RESET);
            vTaskDelay(20);
            return COM_KEY_LEFT_TOP_LONG;
        }
        return COM_KEY_LEFT_TOP;
    }else if (READ_RIGHT_TOP() == GPIO_PIN_RESET){
        uint16_t times = 0;
        vTaskDelay(20);
        while (READ_RIGHT_TOP() == GPIO_PIN_RESET && times < 10){
            times++;
            vTaskDelay(100);
        }
        if (times == 10){
            vTaskDelay(20);
            while (READ_RIGHT_TOP() == GPIO_PIN_RESET);
            vTaskDelay(20);
            return COM_KEY_RIGHT_TOP_LONG;
        }
        return COM_KEY_RIGHT_TOP;
    }else{
        return COM_KEY_NONE;
    }
    
}
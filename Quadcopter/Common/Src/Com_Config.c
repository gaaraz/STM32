#include "Com_Config.h"

/* 四个LED */
LedStruct ledLeftTop = {LED_LEFT_TOP_GPIO_Port, LED_LEFT_TOP_Pin, COM_LEFT_TOP, 0};
LedStruct ledLeftBottom = {LED_LEFT_BOTTOM_GPIO_Port, LED_LEFT_BOTTOM_Pin,COM_LEFT_BOTTOM, 0};
LedStruct ledRightTop = {LED_RIGHT_TOP_GPIO_Port, LED_RIGHT_TOP_Pin,COM_RIGHT_TOP, 0};
LedStruct ledRightBottom = {LED_RIGHT_BOTTOM_GPIO_Port, LED_RIGHT_BOTTOM_Pin, COM_RIGHT_BOTTOM, 0};


/* 遥控器是否连接成功 */
Com_Status isRemoteCoected = COM_FAIL;
/* 遥控器是否解锁 */
Com_Status isRemoteUnlocked = COM_FAIL;
/* 是否开启定高模式 */
Com_Status isFixHeight = COM_FAIL;

MotorStruct motorLeftTop = {COM_LEFT_TOP, 0, &htim3, TIM_CHANNEL_1};
MotorStruct motorLeftBottom = {COM_LEFT_BOTTOM, 0, &htim4, TIM_CHANNEL_4};
MotorStruct motorRightTop = {COM_RIGHT_TOP, 0, &htim2, TIM_CHANNEL_2};
MotorStruct motorRightBottom = {COM_RIGHT_BOTTOM, 0, &htim1, TIM_CHANNEL_3};

ComJoyStick joyStick;

void Com_Config_PrintJoyStick(uint8_t *pre){
    printf("%s thr = %d, yaw = %d, pit = %d, rol = %d\r\n", 
        pre, joyStick.THR, joyStick.YAW, joyStick.PIT, joyStick.ROL);
}


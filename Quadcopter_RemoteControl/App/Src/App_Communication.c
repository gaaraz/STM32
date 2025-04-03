#include "App_Communication.h"

void App_Communication_Start(void){
    debug_printfln("App_Communication_Start");

    /* 初始化SI24R1 */
    Inf_SI24R1_Init();
    while (Inf_SI24R1_Check()){
        HAL_Delay(500);
    }

    Inf_SI24R1_TX_Mode();
    debug_printfln("App_Communication_Start finished");
}

void App_Communication_SendJoyStickData(void){
    uint8_t index = 0;

    TX_BUF[index++] = FRAME_0;
    TX_BUF[index++] = FRAME_1;
    TX_BUF[index++] = FRAME_2;

    TX_BUF[index++] = 0;

    TX_BUF[index++] = joyStick.THR >> 8;
    TX_BUF[index++] = joyStick.THR;

    TX_BUF[index++] = joyStick.YAW >> 8;
    TX_BUF[index++] = joyStick.YAW;

    TX_BUF[index++] = joyStick.PIT >> 8;
    TX_BUF[index++] = joyStick.PIT;

    TX_BUF[index++] = joyStick.ROL >> 8;
    TX_BUF[index++] = joyStick.ROL;

    TX_BUF[index++] = joyStick.isPowerOff;
    joyStick.isPowerOff = 0;

    TX_BUF[index++] = joyStick.isFixHeight;
    joyStick.isFixHeight = 0;

    TX_BUF[3] = index - 4;

    int32_t sum = 0;
    for (int i = 0; i < index; i++){
        sum += TX_BUF[i];
    }
    TX_BUF[index++] = sum >> 24;
    TX_BUF[index++] = sum >> 16;
    TX_BUF[index++] = sum >> 8;
    TX_BUF[index++] = sum;

    taskENTER_CRITICAL();
    Inf_SI24R1_TxPacket(TX_BUF);
    taskEXIT_CRITICAL();
}

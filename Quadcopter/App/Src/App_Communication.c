#include "App_Communication.h"

void App_Communication_Start(void){
    debug_printfln("App_Communication_Start");

    /* 初始化SI24R1 */
    Inf_SI24R1_Init();
    while (Inf_SI24R1_Check()){
        HAL_Delay(500);
    }

    Inf_SI24R1_RX_Mode();
    debug_printfln("App_Communication_Start finished");
}


Com_Status App_Communication_ReceiveJoyStickData(void){
    taskENTER_CRITICAL();
    Com_Status r = Inf_SI24R1_RxPacket(RX_BUF);
    taskEXIT_CRITICAL();
    if ( r == 1) return COM_FAIL;

    if(RX_BUF[0] != FRAME_0 || 
        RX_BUF[1] != FRAME_1 ||
        RX_BUF[2] != FRAME_2)
    {
        return COM_FAIL;
    }

    joyStick.THR = (RX_BUF[4] << 8) | RX_BUF[5];
    joyStick.YAW = (RX_BUF[6] << 8) | RX_BUF[7];
    joyStick.PIT = (RX_BUF[8] << 8) | RX_BUF[9];
    joyStick.ROL = (RX_BUF[10] << 8) | RX_BUF[11];

    joyStick.isPowerOff = RX_BUF[12];
    joyStick.isFixHeight = RX_BUF[13];

    return COM_OK;
}


#include "App_DataProcess.h"

void App_DataProcess_Start(void){
    Inf_JoyStickAndKey_Init();
}


static void App_DataProcess_JoyStickDataTrasfer(void){
    // 将摇杆数据转换为小于1000的值
    joyStick.THR = 1000 - joyStick.THR * 1000 / 4095;
    joyStick.PIT = 1000 - joyStick.PIT * 1000 / 4095;
    joyStick.ROL = 1000 - joyStick.ROL * 1000 / 4095;
    joyStick.YAW = 1000 - joyStick.YAW * 1000 / 4095;
}


void App_DataProcess_JoyStickDataProcess(void){
    taskENTER_CRITICAL();
    Inf_JoyStickAndKey_JoyStickScan();  //读取摇杆数据

    App_DataProcess_JoyStickDataTrasfer();

    // 去除偏置
    joyStick.THR -= joyStickBias.THR;
    joyStick.PIT -= joyStickBias.PIT;
    joyStick.ROL -= joyStickBias.ROL;
    joyStick.YAW -= joyStickBias.YAW;

    joyStick.THR = LIMIT(joyStick.THR, 0, 1000);
    joyStick.PIT = LIMIT(joyStick.PIT, 0, 1000);
    joyStick.ROL = LIMIT(joyStick.ROL, 0, 1000);
    joyStick.YAW = LIMIT(joyStick.YAW, 0, 1000);
    taskEXIT_CRITICAL();
}


static void App_DataProcess_JoyStickCalculateBias(void){
    taskENTER_CRITICAL();
    joyStickBias.THR = 0;
    joyStickBias.PIT = 0;
    joyStickBias.ROL = 0;
    joyStickBias.YAW = 0;

    for (uint8_t i = 0; i < 100; i++)
    {
        //读取摇杆数据
        Inf_JoyStickAndKey_JoyStickScan();  

        // 将摇杆数据转换为小于1000的值
        App_DataProcess_JoyStickDataTrasfer();

        joyStickBias.THR += joyStick.THR;
        joyStickBias.PIT += (joyStick.PIT - 500);
        joyStickBias.ROL += (joyStick.ROL - 500);
        joyStickBias.YAW += (joyStick.YAW - 500);
        vTaskDelay(10);
    }

    joyStickBias.THR /= 100;
    joyStickBias.PIT /= 100;
    joyStickBias.ROL /= 100;
    joyStickBias.YAW /= 100;

    taskEXIT_CRITICAL();
}


void App_DataProcess_KeyDataProcess(void){
    switch(Inf_JoyStickAndKey_KeyScan())
    {
      case COM_KEY_LEFT:
        // debug_printf("left");
        joyStickBias.ROL += 10;
        break;
      case COM_KEY_RIGHT:
        joyStickBias.ROL -= 10;
        break;
      case COM_KEY_UP:
        joyStickBias.PIT -= 10;
        break;
      case COM_KEY_DOWN:
        joyStickBias.PIT += 10;
        break;
      case COM_KEY_LEFT_TOP:
        // debug_printf("poweroff");
        joyStickBias.isPowerOff = 1;
        break;
      case COM_KEY_RIGHT_TOP:
        // debug_printf("isFixHeight");
        joyStickBias.isFixHeight = 1;
        break;
      case COM_KEY_LEFT_TOP_LONG:
        debug_printfln("Left top key long press");
        break;
      case COM_KEY_RIGHT_TOP_LONG:
        App_DataProcess_JoyStickCalculateBias();
        break;
      case COM_KEY_NONE:
        break;
      default:
        break;
    }
}
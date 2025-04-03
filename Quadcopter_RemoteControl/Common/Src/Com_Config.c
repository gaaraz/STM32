#include "Com_Config.h"

ComJoyStick joyStick;

ComJoyStick joyStickBias;

void Com_Config_PrintJoyStick(uint8_t *pre){
    printf("%s thr = %d, yaw = %d, pit = %d, rol = %d\r\n", 
        pre, joyStick.THR, joyStick.YAW, joyStick.PIT, joyStick.ROL);
}

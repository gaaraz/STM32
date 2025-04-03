#include "Inf_Led.h"

void Inf_Led_SetStatus(LedStruct *led){
    if (led->status <= 1)
    {
        HAL_GPIO_WritePin(led->port, led->pin, (GPIO_PinState)led->status);
    }else{
        HAL_GPIO_TogglePin(led->port, led->pin);
    }
}
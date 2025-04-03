#include "Com_Debug.h"


void Com_Debug_Start(void)
{
    // ...
}

int fput(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
    return ch;
}


int _write(int file, char *data, int len) {
    // 使用 HAL 库将数据发送到 USART2
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 1000);
    return len;
}


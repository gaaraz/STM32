#include "Inf_SI24R1.h"

uint8_t ADDRESS[SI24R1_TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};

uint8_t TX_BUF[SI24R1_TX_PLOAD_WIDTH];

/* 初始化 */
void Inf_SI24R1_Init(void){
    SI24R1_CS_HIGH();
    SI24R1_CE_LOW();
    SI24R1_IRQ_HIGH();
}


/* 写寄存器的值（单字节） */
HAL_StatusTypeDef Inf_SI24R1_WriteReg(uint8_t reg, uint8_t value){
    HAL_StatusTypeDef status;

    SI24R1_CS_LOW();
    status = HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
    SI24R1_CS_HIGH();

    return status;
}


/* 写寄存器的值（多字节） */
HAL_StatusTypeDef Inf_SI24R1_WriteBuf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes){
    HAL_StatusTypeDef status;

    SI24R1_CS_LOW();
    status = HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    HAL_SPI_Transmit(&hspi1, pBuf, bytes, 1000);
    SI24R1_CS_HIGH();

    return status;
}


/* 读寄存器的值（单字节） */
uint8_t Inf_SI24R1_ReadReg(uint8_t reg){
    uint8_t value;

    SI24R1_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    HAL_SPI_Receive(&hspi1, &value, 1, 1000);
    SI24R1_CS_HIGH();

    return value;
}



/* 读寄存器的值（多字节） */
HAL_StatusTypeDef Inf_SI24R1_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t bytes){
    HAL_StatusTypeDef status;

    SI24R1_CS_LOW();
    status = HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    HAL_SPI_Receive(&hspi1, pBuf, bytes, 1000);
    SI24R1_CS_HIGH();

    return status;
}


/* 进入接收模式 */
void Inf_SI24R1_RX_Mode(void){
    SI24R1_CE_LOW();

    // 接收设备接收通道0使用和发送设备相同的地址
    Inf_SI24R1_WriteBuf(SI24R1_WRITE_REGISTER + SI24R1_RX_ADDR_P0, ADDRESS, SI24R1_TX_ADR_WIDTH);
    // 使能接收通道0自动应答
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_EN_AA, 0x01);
    // 使能接收通道0
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_EN_RXADDR, 0x01);
    // 选择射频通道
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_RF_CH, CH);
    // 接收通道0选择和发送通道相同有效数据宽度
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_RX_PW_P0, SI24R1_TX_PLOAD_WIDTH);
    // 数据传输速率2Mbps，发射功率7dBm
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_RF_SETUP, 0x0f);
    // CRC使能，16位CRC校验，上电，接收模式
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_CONFIG, 0x0f);
    // 清除所有的·中断标志位
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_STATUS, 0xff);
    // 拉高CE引脚，进入接收模式
    SI24R1_CE_HIGH();
}


/* 进入发送模式 */
void Inf_SI24R1_TX_Mode(void){
    SI24R1_CE_LOW();

    // 写入发送地址
    Inf_SI24R1_WriteBuf(SI24R1_WRITE_REGISTER +SI24R1_TX_ADDR, ADDRESS, SI24R1_TX_ADR_WIDTH);
    // 为了应答接收设备，接收通道0地址和发送地址相同
    Inf_SI24R1_WriteBuf(SI24R1_WRITE_REGISTER +SI24R1_RX_ADDR_P0, ADDRESS, SI24R1_TX_ADR_WIDTH);
    

    // 使能接收通道0自动应答
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_EN_AA, 0x01);
    // 使能接收通道0
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_EN_RXADDR, 0x01);
    // 自动重发延时等待250us+86us，自动重发次数为10次
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_SETUP_RETR, 0x0a);
    // 选择射频通道
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_RF_CH, CH);
    // 数据传输速率2Mbps，发射功率7dBm
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_RF_SETUP, 0x0f);
    // CRC使能，16位CRC校验，上电，接收模式
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_CONFIG, 0x0e);
    SI24R1_CE_HIGH();
}

uint8_t Inf_SI24R1_RxPacket(uint8_t *rxbuf){
    uint8_t status;
    status = Inf_SI24R1_ReadReg(SI24R1_STATUS);
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_STATUS, status); // 清除中断标志位
    if(status & SI24R1_STATUS_RX_DR){
        Inf_SI24R1_ReadBuf(SI24R1_RD_RX_PLOAD, rxbuf, SI24R1_TX_PLOAD_WIDTH);
        return 0;
    }
    return 1;
}

uint8_t Inf_SI24R1_TxPacket(uint8_t *txbuf){
    uint8_t status = 0;
    SI24R1_CE_LOW();
    // 写数据到TX FIFO
    Inf_SI24R1_WriteBuf(SI24R1_WR_TX_PLOAD, txbuf, SI24R1_TX_PLOAD_WIDTH);  
    // 拉高CE引脚，进入发送模式
    SI24R1_CE_HIGH();

    
    // while (SI24R1_IRQ_READ() == GPIO_PIN_SET) // 等待发送完成

    while (!(status & (SI24R1_STATUS_TX_DS | SI24R1_STATUS_MAX_RT)))
    {
        status = Inf_SI24R1_ReadReg(SI24R1_READ_REGISTER + SI24R1_STATUS);
    }
    
    Inf_SI24R1_WriteReg(SI24R1_WRITE_REGISTER + SI24R1_STATUS, status); // 清除中断标志位
    if (status & SI24R1_STATUS_MAX_RT){
        if (status & 0x01)
        {
            Inf_SI24R1_WriteReg(SI24R1_FLUSH_TX, 0xff); // 清除发送FIFO
        }
        return SI24R1_STATUS_MAX_RT;
    }
    
    if (status & SI24R1_STATUS_TX_DS){
        return SI24R1_STATUS_TX_DS;
    }
    return 0xFF;
}


uint8_t Inf_SI24R1_Check(void){
    uint8_t buff_w[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    uint8_t buff_r[5] = {0};
    uint8_t count = 0;

    Inf_SI24R1_WriteBuf(SI24R1_WRITE_REGISTER + SI24R1_TX_ADDR, buff_w, 5);
    Inf_SI24R1_ReadBuf(SI24R1_READ_REGISTER + SI24R1_TX_ADDR, buff_r, 5);

    for (int i = 0; i < 5; i++){
        if (buff_w[i] == buff_r[i]){
            count++;
        }
    }
    if (count == 5){
        return 0; // 检测成功
    }else{
        return 1; // 检测失败
    }
}



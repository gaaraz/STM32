#ifndef __INF_SI24R1_H__
#define __INF_SI24R1_H__

#include "Com_Config.h"
#include "Com_Debug.h"
#include "spi.h"
#include "main.h"


#define CH 111

#define SI24R1_CS_HIGH()    HAL_GPIO_WritePin(SI24R1_CS_GPIO_Port, SI24R1_CS_Pin, GPIO_PIN_SET)
#define SI24R1_CS_LOW()     HAL_GPIO_WritePin(SI24R1_CS_GPIO_Port, SI24R1_CS_Pin, GPIO_PIN_RESET)
#define SI24R1_CE_HIGH()    HAL_GPIO_WritePin(SI24R1_CE_GPIO_Port, SI24R1_CE_Pin, GPIO_PIN_SET)
#define SI24R1_CE_LOW()     HAL_GPIO_WritePin(SI24R1_CE_GPIO_Port, SI24R1_CE_Pin, GPIO_PIN_RESET)
#define SI24R1_IRQ_HIGH()   HAL_GPIO_WritePin(SI24R1_IRQ_GPIO_Port, SI24R1_IRQ_Pin, GPIO_PIN_SET)
#define SI24R1_IRQ_READ()   HAL_GPIO_ReadPin(SI24R1_IRQ_GPIO_Port, SI24R1_IRQ_Pin)

#define SI24R1_TX_ADR_WIDTH 5
#define SI24R1_TX_PLOAD_WIDTH 18

/* SI24R1命令 */
#define SI24R1_READ_REGISTER        0x00    // 读寄存器
#define SI24R1_WRITE_REGISTER       0x20    // 写寄存器
#define SI24R1_RD_RX_PLOAD_WIDTH    0x60    // 读接收数据宽度
#define SI24R1_RD_RX_PLOAD          0x61    // 读接收数据
#define SI24R1_WR_TX_PLOAD          0xA0    // 写发送数据
#define SI24R1_FLUSH_TX             0xE1    // 清空FIFO寄存器发送数据
#define SI24R1_FLUSH_RX             0xE2    // 清空FIFO寄存器接收数据
#define SI24R1_REUSE_TX_PL          0xE3    // 重新使用上一次数据，CE为高，数据包被不断发送
#define SI24R1_NOP                  0xFF    // NOP命令，读状态寄存器

/* SI24R1寄存器地址 */
#define SI24R1_CONFIG               0x00    // 配置寄存器
#define SI24R1_EN_AA                0x01    // 使能自动应答寄存器
#define SI24R1_EN_RXADDR            0x02    // 使能接收地址寄存器
#define SI24R1_SETUP_AW             0x03    // 配置地址宽度寄存器
#define SI24R1_SETUP_RETR           0x04    // 重发配置寄存器
#define SI24R1_RF_CH                0x05    // 射频通道寄存器
#define SI24R1_RF_SETUP             0x06    // 射频设置寄存器
#define SI24R1_STATUS               0x07    // 状态寄存器
#define SI24R1_OBSERVE_TX           0x08    // 观察发送寄存器
#define SI24R1_RSSI                 0x09    // 载波检测寄存器
#define SI24R1_RX_ADDR_P0           0x0A    // 接收地址管道0寄存器
#define SI24R1_RX_ADDR_P1           0x0B    // 接收地址管道1寄存器
#define SI24R1_RX_ADDR_P2           0x0C    // 接收地址管道2寄存器
#define SI24R1_RX_ADDR_P3           0x0D    // 接收地址管道3寄存器
#define SI24R1_RX_ADDR_P4           0x0E    // 接收地址管道4寄存器
#define SI24R1_RX_ADDR_P5           0x0F    // 接收地址管道5寄存器
#define SI24R1_TX_ADDR              0x10    // 发送地址寄存器
#define SI24R1_RX_PW_P0             0x11    // 接收数据宽度管道0寄存器
#define SI24R1_RX_PW_P1             0x12    // 接收数据宽度管道1寄存器
#define SI24R1_RX_PW_P2             0x13    // 接收数据宽度管道2寄存器
#define SI24R1_RX_PW_P3             0x14    // 接收数据宽度管道3寄存器
#define SI24R1_RX_PW_P4             0x15    // 接收数据宽度管道4寄存器
#define SI24R1_RX_PW_P5             0x16    // 接收数据宽度管道5寄存器
#define SI24R1_FIFO_STATUS          0x17    // FIFO状态寄存器

/* 状态寄存器 */
#define SI24R1_STATUS_RX_DR         0x40    // 接收数据准备好
#define SI24R1_STATUS_TX_DS         0x20    // 发送数据准备好
#define SI24R1_STATUS_MAX_RT        0x10    // 达到最大重发次数

extern uint8_t RX_BUF[SI24R1_TX_PLOAD_WIDTH];

void Inf_SI24R1_Init(void);
HAL_StatusTypeDef Inf_SI24R1_WriteReg(uint8_t reg, uint8_t value);
HAL_StatusTypeDef Inf_SI24R1_WriteBuf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes);
uint8_t Inf_SI24R1_ReadReg(uint8_t reg);
HAL_StatusTypeDef Inf_SI24R1_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);

void Inf_SI24R1_RX_Mode(void);
void Inf_SI24R1_TX_Mode(void);
uint8_t Inf_SI24R1_RxPacket(uint8_t *rxbuf);
uint8_t Inf_SI24R1_TxPacket(uint8_t *txbuf);

uint8_t Inf_SI24R1_Check(void);

#endif



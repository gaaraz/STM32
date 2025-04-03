#include "driver_oled_hardware.h"
#include "OLED_Font.h"

static uint8_t oledBuffer[FRAME_BUFFER_SIZE];

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	/*
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
	 */
	uint8_t temp[2];

	temp[0] = 0x00;
	temp[1] = Command;

	HAL_I2C_Master_Transmit(&hi2c1, 0x78, temp, 2, 500);
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	/*
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
	 */
	
	uint8_t temp[2];

	temp[0] = 0x40;
	temp[1] = Data;

	HAL_I2C_Master_Transmit(&hi2c1, 0x78, temp, 2, 500);
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i = 0;
    uint8_t buf[128] = {0};
    
    for(i=0; i<8; i++)
    {
        OLED_SetCursor(i, 0);
        OLED_WriteNBytes(&buf[0], 128);
    }
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

void OLED_Init(void){
    // uint32_t i, j;
	
	// for (i = 0; i < 1000; i++)			//上电延时
	// {
	// 	for (j = 0; j < 1000; j++);
	// }

	// MX_I2C1_Init();
    // HAL_I2C_MspInit(&hi2c1);

	OLED_WriteCommand(0x20);	//设置地址模式
    OLED_WriteCommand(0x02);

	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);	//设置显示开始行

	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置

	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x02);

	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0x7F);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0xAF);	//开启显示



    // OLED_WriteCommand(0xAE);	//关闭显示
	// OLED_WriteCommand(0x12);
	
	// OLED_WriteCommand(0xD9);	//设置预充电周期
	// OLED_WriteCommand(0xF1);

	// OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	// OLED_WriteCommand(0x30);
	
	OLED_Clear();				//OLED清屏
}

/*
 *  函数名：OLED_WriteNBytes
 *  功能描述：连续发送N个写入显存的数据
 *  输入参数：buf         - 数据buffer
 *            length - 数据个数
 *  输出参数：无
 *  返回值：0-成功, 其他值失败
 */
static int OLED_WriteNBytes(uint8_t *buf, uint16_t length)
{
    return HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, 1, buf, length, 500);
}


/**********************************************************************
 *  OLED_FlushRegion
 *  功能描述：刷新OLED的区域
 *  输入参数：col     - OLED的列,取值范围0~127
 *            row - OLED的行,取值范围0~63
 *            width - 宽度
 *            heigh - 高度,必须是8的整数倍
 *  输出参数：无
 *  返回值：无
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void OLED_FlushRegion(int col, int row, int width, int heigh){
	uint8_t i;
    uint8_t page = row / 8;
    uint8_t page_cnt = ((row % 8) + heigh +7)/8;
    
    for (i = 0; i < page_cnt; i++)
    {
        if (page+i < 8)
        {
           OLED_SetCursor(page+i, col);
           OLED_WriteNBytes(&oledBuffer[col + (page + i) * FRAME_WIDTH], width);
        }
      }
}

/**********************************************************************
 *  OLED_Flush
 *  功能描述：把Framebuffer的数据全部刷新到OLED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void OLED_Flush(void){
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_WriteCommand(0xb0+i);	//设置行起始地址
		OLED_WriteCommand(0x00);	//设置低列起始地址
		OLED_WriteCommand(0x10);	//设置高列起始地址
		OLED_WriteNBytes(&oledBuffer[128*i], 128);
	}
	
}

/**********************************************************************
 *  OLED_GetFrameBuffer
 *  功能描述：获得OLED的Framebuffer
 *  输入参数：无
 *  输出参数：pXres/pYres/pBpp - 用来保存分辨率、bpp
 *  返回值：Framebuffer首地址
 * -----------------------------------------------
 * 2023/08/31        V1.0     韦东山       创建
 ***********************************************************************/
void * OLED_GetFrameBuffer(uint32_t *pXres, uint32_t *pYres, uint32_t *pBpp)
{
    *pXres = FRAME_WIDTH;
    *pYres = FRAME_HEIGHT;
    *pBpp  = 1;
    return oledBuffer;
}

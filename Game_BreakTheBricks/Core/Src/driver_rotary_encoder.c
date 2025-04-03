#include "driver_rotary_encoder.h"
#include "game.h"


static int32_t g_count = 0;
static int32_t g_speed = 0; /* 速度(正数表示顺时针旋转,负数表示逆时针旋转,单位:每秒转动次数) */

static QueueHandle_t g_xQueueRotary;
static uint8_t g_ucQueueRotaryBuf[ROTARY_QUEUE_LEN * sizeof(RotaryData)];
static StaticQueue_t g_xQueueRotaryStaticStruct;


/**********************************************************************
 * 函数名称： GetQueueRotary
 * 功能描述： 返回旋转编码器驱动程序里的队列句柄
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 队列句柄
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/09/02	     V1.0	  韦东山	      创建
 ***********************************************************************/
QueueHandle_t GetQueueRotary(void)
{
	return g_xQueueRotary;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S1
 * 功能描述： 读取旋转编码器S1引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S1(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
        return 0;
    else
        return 1;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S2
 * 功能描述： 读取旋转编码器S2引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S2(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13))
        return 0;
    else
        return 1;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_IRQ_Callback
 * 功能描述： 旋转编码器的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_IRQ_Callback(void){
    uint64_t time;
    static uint64_t pre_time = 0;
    RotaryData rdata;

    time = system_get_ns();

    // 抖动处理
    if(time - pre_time < 2000000){
        pre_time = time;
        return;
    }

    if (!RotaryEncoder_Get_S1())
    {
        return;
    }
    
    g_speed = (uint64_t)1000000000/(time - pre_time);
    if (g_speed == 0)
    {
        g_speed = 1;
    }
    
    if (RotaryEncoder_Get_S2())
    {
        // 顺时针
        g_count++;
    }else{
        // 逆时针
        g_count--;
        g_speed = 0 - g_speed;
    }
    pre_time = time;

    rdata.cnt = g_count;
    rdata.speed = g_speed;
    xQueueSendFromISR(g_xQueueRotary, &rdata, NULL);
}

// void RotaryEncoder_IRQ_Callback(void){
//     mdelay(2);
//     if (!RotaryEncoder_Get_S1())
//     {
//         return;
//     }
    
//     if (RotaryEncoder_Get_S2())
//     {
//         /* 顺时针 */
//         btnRight();
//     }else{
//         /* 逆时针 */
//         btnLeft();
//     }
    
// }

/**********************************************************************
 * 函数名称： RotaryEncoder_Init
 * 功能描述： 旋转编码器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_Init(void){
    g_xQueueRotary = xQueueCreateStatic(ROTARY_QUEUE_LEN, sizeof(RotaryData), g_ucQueueRotaryBuf, &g_xQueueRotaryStaticStruct);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case GPIO_PIN_12:
        {
            RotaryEncoder_IRQ_Callback();
            break;
        }

        case GPIO_PIN_10:
        {
            IR_RECEIVER_IRQ_Callback();
            break;
        }

        default:
        {
            break;
        }
    }
}



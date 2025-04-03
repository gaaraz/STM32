#include "driver_ir_receiver.h"

static unsigned char g_KeysBuf[128];
static int g_KeysBuf_R,g_KeysBuf_W;             //g_KeysBuf_R-环形缓冲区读位置,g_KeysBuf_W-环形缓冲区写位置

static uint64_t g_IRReceiverIRQ_Timers[68];
static int g_IRReceiverIRQ_Cnt = 0;

static QueueHandle_t g_xQueueIR;

#define NEXT_POS(x) ((x+1) % 128)


void IRReceiver_Init(void){
    g_xQueueIR = xQueueCreate(IR_QUEUE_LEN, sizeof(IRData));
}

QueueHandle_t GetQueueIR(void){
    return g_xQueueIR;
}

/* 判断环形缓冲区是否为空(读地址等于写地址) */
static int isKeysBufEmpty(void){
    return (g_KeysBuf_R == g_KeysBuf_W);
}

/* 判断环形缓冲区是否为满(读地址等于写地址+1) */
static int isKeysBufFull(void){
    return (g_KeysBuf_R == NEXT_POS(g_KeysBuf_W));
}

static void PutKeyToBuf(unsigned char key){
    if (!isKeysBufFull())
    {
        g_KeysBuf[g_KeysBuf_W] = key;
        g_KeysBuf_W = NEXT_POS(g_KeysBuf_W);
    }
}

static unsigned char GetKeyFromBuf(void){
    unsigned char key = 0xff;
    if (!isKeysBufEmpty())
    {
        key = g_KeysBuf[g_KeysBuf_R];
        g_KeysBuf_R = NEXT_POS(g_KeysBuf_R);
    }
    return key;
}

static int isRepeatedKey(void)
{
	uint64_t time;

	/* 1. 判断重复码 : 9ms的低脉冲, 2.25ms高脉冲  */
	time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
	if (time < 8000000 || time > 10000000)
	{
		return 0;
	}

	time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
	if (time < 2000000 || time > 2500000)
	{
		return 0;
	}	

	return 1;
}

static int IRReceiver_IRQTimes_Parse(void){
    uint64_t time;
    int i;
    int m,n;
    unsigned char datas[4];
    unsigned char data = 0;
    int bits = 0;
    int byte = 0;
    IRData irdata;

    /* 判断引导码:9ms的低脉冲,4.5ms的高脉冲 */
    time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
    if (time < 8000000 || time > 10000000)
    {
        return -1;
    }
    
    /* 4.5ms的高脉冲 */
    time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
    if (time < 3500000 || time > 5500000)
    {
        return -1;
    }
    
    for (i = 0; i < 32; i++)
    {
        m = 3 + i * 2;
        n = m + 1;
        time = g_IRReceiverIRQ_Timers[n] - g_IRReceiverIRQ_Timers[m];
        data <<= 1;
        bits++;
        if (time > 1000000)
        {
            data |= 1;
        }
        
        if (bits == 8)
        {
            datas[byte] = data;
            byte++;
            data = 0;
            bits = 0;
        }
    }
    
    datas[1] = ~datas[1];
    datas[3] = ~datas[3];

    if (datas[0] != datas[1] || datas[2] != datas[3])
    {
        g_IRReceiverIRQ_Cnt = 0;
        return -1;
    }
    
    irdata.dev = datas[0];
    irdata.val = datas[2];
    xQueueSendFromISR(g_xQueueIR, &irdata, NULL);
    // PutKeyToBuf(datas[0]);
    // PutKeyToBuf(datas[2]);
    
    return 0;
}


void IR_RECEIVER_IRQ_Callback(void){
    uint64_t time;
    static uint64_t pre_time = 0;
    IRData irdata;

    time = system_get_ns();

    /* 一次按键的最长数据 = 引导码 + 32个数据"1" = 9+4.5+2.25*32 = 85.5ms
     * 如果当前中断的时刻, 举例上次中断的时刻超过这个时间, 以前的数据就抛弃
     */
    if(time - pre_time > 100000000){
        g_IRReceiverIRQ_Cnt = 0;
    }
    pre_time = time;

    g_IRReceiverIRQ_Timers[g_IRReceiverIRQ_Cnt] = time;
    g_IRReceiverIRQ_Cnt++;

    if (g_IRReceiverIRQ_Cnt == 4)
    {
        if (isRepeatedKey()){
            irdata.dev = 0x00;
            irdata.val = 0x00;
            xQueueSendFromISR(g_xQueueIR, &irdata, NULL);
            g_IRReceiverIRQ_Cnt = 0;
        }
    }
    

    if (g_IRReceiverIRQ_Cnt == 68)
    {
        IRReceiver_IRQTimes_Parse();
        g_IRReceiverIRQ_Cnt = 0;
    }
    
}

int IRReceiver_Read(uint8_t *pDev, uint8_t *pData)
{
    if (isKeysBufEmpty())
        return -1;
    
    *pDev  = GetKeyFromBuf();
    *pData = GetKeyFromBuf();
    return 0;
}

const char *IRReceiver_GetKeyName(uint8_t key)
{
    const uint8_t codes[] = {0xa2, 0x62, 0xe2, 0x22, 0x02, 0xC2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0,
        0x18, 0x4a, 0x10, 0x5a, 0x38};
    
    const char *names[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#", 
        "Up", "Down", "Left", "Right", "OK"};
    
    int i;
    for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++)
    {
        if (key == codes[i])
        {
            return names[i];
        }
    }
    return "Error";
}

void IR_Receiver_Test(void){
    uint8_t dev, data;

    while (1)
    {
        OLED_ShowString(1, 1, "IR_RECEIVER_DATA:");
        if (!IRReceiver_Read(&dev, &data))
        {
            OLED_ShowHexNum(4, 1, dev, 2);
            OLED_ShowHexNum(8, 1, data, 2);
        }
        
    }
    
}


// void IR_RECEIVER_IRQ_Callback(){
//     // cnt++;
//     GPIO_PinState state = HAL_GPIO_ReadPin(IR_RECEIVER_GPIO_Port, IR_RECEIVER_Pin);   
//     // OLED_ShowNum((cnt/16)*4+1, cnt%16, state, 1);
//     if (IR_RECEIVER_STARTED == 0)
//     {
//         if (state == GPIO_PIN_RESET)
//         {
//             __HAL_TIM_SET_COUNTER(&htim3,0);
//             HAL_TIM_Base_Start_IT(&htim3);
//             return;
//         }else{
//             HAL_TIM_Base_Stop_IT(&htim3);
//             count = __HAL_TIM_GET_COUNTER(&htim3);
//             if (count>8000 && count<10000)
//             {
//                 IR_RECEIVER_STARTED = 1;
//                 __HAL_TIM_SET_COUNTER(&htim3,0);
//                 HAL_TIM_Base_Start_IT(&htim3);
//             }
//             return;           
//         } 
//     }
//     else if (IR_RECEIVER_STARTED == 1)
//     {
//         if (state == GPIO_PIN_RESET)
//         {
//             HAL_TIM_Base_Stop_IT(&htim3);
//             count = __HAL_TIM_GET_COUNTER(&htim3);
//             if (count>3500 && count<5500){
//                 IR_RECEIVER_STARTED = 2;
//             }else{
//                 IR_RECEIVER_STARTED = 0;
//             }
//             return; 
//         }
//         else
//         {
//             IR_RECEIVER_STARTED = 0;
//             return; 
//         }
//     }
//     else if (IR_RECEIVER_STARTED == 2)
//     {
//         if (state == GPIO_PIN_SET){
//             __HAL_TIM_SET_COUNTER(&htim3,0);
//             HAL_TIM_Base_Start_IT(&htim3);
//             return; 
//         }
//         else
//         {
//             HAL_TIM_Base_Stop_IT(&htim3);
//             count = __HAL_TIM_GET_COUNTER(&htim3);
//             if (count>460 && count<660){
//                 __HAL_TIM_SET_COUNTER(&htim3,0);
//                 HAL_TIM_Base_Start_IT(&htim3);
//             }else if (count>1580 && count<1780){
//                 data[cnt/8] |= 1<<(cnt%8);
//                 __HAL_TIM_SET_COUNTER(&htim3,0);
//                 HAL_TIM_Base_Start_IT(&htim3);
//             }else{
//                 IR_RECEIVER_STARTED = 0;
//             }
//             if (cnt < 31)
//             {
//                 cnt++;
//             }
//             return; 
//         }
//     }
// }

// void IR_RECEIVER_Callback(void){
//     HAL_TIM_Base_Stop_IT(&htim3);
//     memset(data, 0, sizeof(data));
// 	IR_RECEIVER_STARTED = 0;
//     cnt = 0;
// }

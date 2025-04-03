#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__


/* Includes */
#include "main.h"
#include "tim.h"

/* define */
#define LIMIT(x, min, max)  ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define FRAME_0  (0xAA)
#define FRAME_1  (0xBB)
#define FRAME_2  (0xCC)

/* enum */
typedef enum
{
    COM_OK = 0,
    COM_TIMEOUT,
    COM_FAIL,
} Com_Status;

typedef enum
{
    COM_LEFT_TOP,
    COM_LEFT_BOTTOM,
    COM_RIGHT_TOP,
    COM_RIGHT_BOTTOM,
} Com_Location;

/* struct */
typedef struct
{
    GPIO_TypeDef *port;         // 灯的端口
    uint16_t pin;               // 灯的引脚
    Com_Location location;      // 灯的位置
    uint8_t status;             // 灯的状态：0-常亮，1-熄灭，2+-闪烁周期
} LedStruct;

typedef struct
{
    Com_Location location;          // 电机的位置
    int16_t speed;                  // 电机的速度
    TIM_HandleTypeDef* p_htim;      // 定时器句柄
    uint32_t channel;               // 定时器通道
} MotorStruct;

typedef struct{
    int16_t THR;
    int16_t PIT;
    int16_t ROL;
    int16_t YAW;

    uint8_t isPowerOff;
    uint8_t isFixHeight;
} ComJoyStick;


/* extern */
extern ComJoyStick joyStick;

extern LedStruct ledLeftTop;
extern LedStruct ledLeftBottom;
extern LedStruct ledRightTop;
extern LedStruct ledRightBottom;


extern Com_Status isRemoteCoected;
extern Com_Status isRemoteUnlocked;
extern Com_Status isFixHeight;


extern MotorStruct motorLeftTop;
extern MotorStruct motorLeftBottom;
extern MotorStruct motorRightTop;
extern MotorStruct motorRightBottom;

void Com_Config_PrintJoyStick(uint8_t *pre);

#endif 
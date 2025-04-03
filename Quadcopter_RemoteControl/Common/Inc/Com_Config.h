#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__


/* Includes */
#include "main.h"
#include "Com_Debug.h"

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
    COM_KEY_NONE = 0,
    COM_KEY_LEFT,
    COM_KEY_UP,
    COM_KEY_DOWN,
    COM_KEY_RIGHT,
    COM_KEY_LEFT_TOP,
    COM_KEY_RIGHT_TOP,
    COM_KEY_LEFT_TOP_LONG,
    COM_KEY_RIGHT_TOP_LONG
} Com_Key;

/* struct */
typedef struct{
    int16_t THR;
    int16_t PIT;
    int16_t ROL;
    int16_t YAW;

    uint8_t isPowerOff;
    uint8_t isFixHeight;
} ComJoyStick;


extern ComJoyStick joyStick;
extern ComJoyStick joyStickBias;

void Com_Config_PrintJoyStick(uint8_t *pre);
#endif
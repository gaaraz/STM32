#ifndef __DRAW_H__
#define __DRAW_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "typedefs.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
void draw_string_P(const char*, bool, byte, byte);
void draw_string(char*, bool, byte, byte);
//void draw_string_time(char*, bool, byte, byte);
void draw_bitmap(byte x, byte yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY);
void draw_flushArea(byte x, byte y, byte w, byte h);
void draw_clearArea(byte, byte, byte);//, byte);
void draw_end(void);
void draw_init(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif

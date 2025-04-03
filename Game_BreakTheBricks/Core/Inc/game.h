#ifndef __GAME_H__
#define __GAME_H__

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

// void game_init(void);

void game_draw(void);

void game_task(void *param);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif


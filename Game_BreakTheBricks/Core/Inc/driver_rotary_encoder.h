#ifndef __DRIVER_ROTARY_ENCODER_H__
#define __DRIVER_ROTARY_ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */
#include "driver_timer.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "driver_ir_receiver.h"
#include "driver_mpu6050.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define ROTARY_QUEUE_LEN 10
typedef struct rotary_data {
	int32_t cnt;
	int32_t speed;
} RotaryData;
/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void RotaryEncoder_Init(void);

QueueHandle_t GetQueueRotary(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif

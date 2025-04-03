#ifndef __DRIVER_IR_RECEIVER_H__
#define __DRIVER_IR_RECEIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* USER CODE BEGIN Includes */
#include "string.h"
#include "driver_timer.h"
#include "driver_oled_hardware.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define IR_RECEIVER_GPIO_Port   GPIOA
#define IR_RECEIVER_Pin         GPIO_PIN_8
#define IR_QUEUE_LEN            10
typedef struct ir_data {
	uint32_t dev;
	uint32_t val;
} IRData;
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */


void IRReceiver_Init(void);
QueueHandle_t GetQueueIR(void);
void IR_RECEIVER_IRQ_Callback(void);
void IR_Receiver_Test(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif

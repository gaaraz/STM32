/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Com_Debug.h"
#include "Com_Config.h"
#include "Inf_IP5305T.h"
#include "Inf_Led.h"
#include "Inf_Motor.h"
#include "Inf_SI24R1.h"

#ifndef SI24R1_TX_PLOAD_WIDTH
#define SI24R1_TX_PLOAD_WIDTH 32 // Define a default value, adjust as needed
#endif
#include "App_Flight.h"
#include "App_Communication.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void startTask(void *args);
#define START_TASK_NAME "start_task"
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 10
TaskHandle_t startTaskHandle;

/* 电源管理任务 */
void powerTask(void *args);
#define POWER_TASK_NAME "power_task"
#define POWER_TASK_STACK 128
#define POWER_TASK_PRIORITY 9
TaskHandle_t powerTaskHandle;
#define POWER_CYCLE (10*1000) // 10s

/* led灯任务 */
void ledTask(void *args);
#define LED_TASK_NAME "led_task"
#define LED_TASK_STACK 128
#define LED_TASK_PRIORITY 2
TaskHandle_t ledTaskHandle;
#define LED_CYCLE 50 // 50ms

/* 飞行任务，控制电机 */
void flightTask(void *args);
#define FLIGHT_TASK_NAME "flight_task"
#define FLIGHT_TASK_STACK 256
#define FLIGHT_TASK_PRIORITY 8
TaskHandle_t flightTaskHandle;
#define FLIGHT_CYCLE 4 

/* 通信任务，接收遥控器数据 */
void communicationTask(void *args);
#define COMMUNICATION_TASK_NAME "communication_task"
#define COMMUNICATION_TASK_STACK 256
#define COMMUNICATION_TASK_PRIORITY 8
TaskHandle_t communicationTaskHandle;
#define COMMUNICATION_CYCLE 6 

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  App_Flight_Start();
  App_Communication_Start();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  debug_printfln("start create start task");

  BaseType_t r = xTaskCreate(
      startTask,
      START_TASK_NAME,
      START_TASK_STACK,
      NULL,
      START_TASK_PRIORITY,
      &startTaskHandle
  );
  if (r != pdPASS) {
      debug_printfln("Failed to create start task");
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
//   /* Infinite loop */
//   for(;;)
//   {
//     osDelay(1);
//   }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void startTask(void *args){
  debug_printfln("Start task started");
  /* 创建电源任务 */
  xTaskCreate(
      powerTask,
      POWER_TASK_NAME,
      POWER_TASK_STACK,
      NULL,
      POWER_TASK_PRIORITY,
      &powerTaskHandle
  );

  /* 创建led灯任务 */
  xTaskCreate(
      ledTask,
      LED_TASK_NAME,
      LED_TASK_STACK,
      NULL,
      LED_TASK_PRIORITY,
      &ledTaskHandle
  );

  /* 创建飞行任务 */
  xTaskCreate(
      flightTask,
      FLIGHT_TASK_NAME,
      FLIGHT_TASK_STACK,
      NULL,
      FLIGHT_TASK_PRIORITY,
      &flightTaskHandle
  );

  /* 创建通信任务 */
  xTaskCreate(
      communicationTask,
      COMMUNICATION_TASK_NAME,
      COMMUNICATION_TASK_STACK,
      NULL,
      COMMUNICATION_TASK_PRIORITY,
      &communicationTaskHandle
  );
  debug_printfln("Start task finished");

  vTaskDelete(NULL);
}

/* 电源管理任务 */
void powerTask(void *args){
  while (1)
  {
      /* 关机 */
      if (ulTaskNotifyTake(pdTRUE, POWER_CYCLE) == pdTRUE) {
          debug_printfln("Power off");
          Inf_IP5305T_Close();
      }else{
          /* 开机 */
          debug_printfln("Power on");
          Inf_IP5305T_Open();
      }
  }
}

/* led灯任务 */
void ledTask(void *args){
  debug_printfln("Led task started");

  isRemoteCoected = COM_OK;
  isRemoteUnlocked = COM_OK;
  isFixHeight = COM_FAIL;

  uint32_t preTime = xTaskGetTickCount();
  uint32_t cnt = 0;

  while (1)
  {
      /* 定高、解锁、连接 */
      if (isFixHeight == COM_OK) {
        ledLeftTop.status = 
          ledLeftBottom.status = 
            ledRightTop.status = 
              ledRightBottom.status = 2;
      }else if (isRemoteUnlocked == COM_OK) {
        ledLeftTop.status = 
          ledLeftBottom.status = 
            ledRightTop.status = 
              ledRightBottom.status = 10;
      }else if (isRemoteCoected == COM_OK) {
        ledLeftTop.status = 
          ledLeftBottom.status = 
            ledRightTop.status = 
              ledRightBottom.status = 20;
      }else{
        ledLeftTop.status = 
          ledLeftBottom.status = 
            ledRightTop.status = 
              ledRightBottom.status = 0;
      }

      cnt++;

      if(ledLeftTop.status <=1 || cnt % ledLeftTop.status == 0){
        Inf_Led_SetStatus(&ledLeftTop);
        Inf_Led_SetStatus(&ledLeftBottom);
        Inf_Led_SetStatus(&ledRightTop);
        Inf_Led_SetStatus(&ledRightBottom);
      }

      vTaskDelayUntil(&preTime, LED_CYCLE);
  }
}


/* 飞行任务，控制电机 */
void flightTask(void *args){
  debug_printfln("Flight task started");

  motorLeftTop.speed = 0;
  motorLeftBottom.speed = 0;
  motorRightTop.speed = 0;
  motorRightBottom.speed = 0;

  uint32_t preTime = xTaskGetTickCount();
  while (1)
  {
      /* 电机控制 */
      Inf_Motor_AllMotorsWork();

      vTaskDelayUntil(&preTime, FLIGHT_CYCLE);
  }
}


/* 通信任务，接收遥控器数据 */
void communicationTask(void *args){
  debug_printfln("Communication task started");
  
  uint32_t preTime = xTaskGetTickCount();
  while (1)
  {
      /* 接收遥控器数据 */
      Com_Status isReceiveData = App_Communication_ReceiveJoyStickData();

      if (isReceiveData == COM_OK) {
        if(joyStick.isPowerOff) {
          xTaskNotifyGive(powerTaskHandle);
        }
      }
      // Com_Config_PrintJoyStick("joyStick");
      /* 延时 */
      vTaskDelayUntil(&preTime, COMMUNICATION_CYCLE);
  }
}

/* USER CODE END Application */


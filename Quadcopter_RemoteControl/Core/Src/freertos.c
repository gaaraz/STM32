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
#include "Inf_SI24R1.h"
#include "Inf_JoyStick_Key.h"
#include "App_Communication.h"
#include "App_DataProcess.h"
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

/* 通信任务，接收遥控器数据 */
void communicationTask(void *args);
#define COMMUNICATION_TASK_NAME "communication_task"
#define COMMUNICATION_TASK_STACK 256
#define COMMUNICATION_TASK_PRIORITY 8
TaskHandle_t communicationTaskHandle;
#define COMMUNICATION_CYCLE 8

/* 按键扫描任务 */
void keyTask(void *args);
#define KEY_TASK_NAME "key_task"
#define KEY_TASK_STACK 256
#define KEY_TASK_PRIORITY 7
TaskHandle_t keyTaskHandle;
#define KEY_CYCLE 40

/* 摇杆扫描任务 */
void joyStickTask(void *args);
#define JOYSTICK_TASK_NAME "joyStick_task"
#define JOYSTICK_TASK_STACK 256
#define JOYSTICK_TASK_PRIORITY 7
TaskHandle_t joyStickTaskHandle;
#define JOYSTICK_CYCLE 4
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
  App_Communication_Start();
  App_DataProcess_Start();
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
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

  /* 创建通信任务 */
  xTaskCreate(
      communicationTask,
      COMMUNICATION_TASK_NAME,
      COMMUNICATION_TASK_STACK,
      NULL,
      COMMUNICATION_TASK_PRIORITY,
      &communicationTaskHandle
  );

  /* 创建按键任务 */
  xTaskCreate(
      keyTask,
      KEY_TASK_NAME,
      KEY_TASK_STACK,
      NULL,
      KEY_TASK_PRIORITY,
      &keyTaskHandle
  );
  debug_printfln("Start task finished");

  xTaskCreate(
    joyStickTask,
    JOYSTICK_TASK_NAME,
    JOYSTICK_TASK_STACK,
    NULL,
    JOYSTICK_TASK_PRIORITY,
    &joyStickTaskHandle
  );

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

/* 通信任务，接收遥控器数据 */
void communicationTask(void *args){
  vTaskDelay(1000);
  debug_printfln("Communication task started");
  
  uint32_t preTime = xTaskGetTickCount();
  while (1)
  {
    debug_printf("power: %d\n", joyStick.isPowerOff);
    /* 发送遥控器数据 */
    App_Communication_SendJoyStickData();

      /* 延时 */
    vTaskDelayUntil(&preTime, COMMUNICATION_CYCLE);
  }
}


void keyTask(void *args){
  debug_printfln("Key task started");
  
  uint32_t preTime = xTaskGetTickCount();
  while (1){
    debug_printf("key");
    App_DataProcess_KeyDataProcess();
    vTaskDelayUntil(&preTime, KEY_CYCLE);
  }
}

void joyStickTask(void *args){
  vTaskDelay(500);
  debug_printfln("JoyStick task started");

  uint32_t preTime = xTaskGetTickCount();
  while (1){
    debug_printfln("JoyStick task...");
    App_DataProcess_JoyStickDataProcess();
    vTaskDelayUntil(&preTime, JOYSTICK_CYCLE);
  }
}

/* USER CODE END Application */


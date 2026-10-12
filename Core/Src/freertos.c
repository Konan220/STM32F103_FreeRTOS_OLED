/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void redTask(void const * argument);
void greenTask(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* 创建红灯任务：亮1000ms / 灭1000ms */
  osThreadDef(redTask, redTask, osPriorityNormal, 0, 128);
  osThreadCreate(osThread(redTask), NULL);

  /* 创建绿灯任务：亮300ms / 灭1500ms */
  osThreadDef(greenTask, greenTask, osPriorityNormal, 0, 128);
  osThreadCreate(osThread(greenTask), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  // 1. OLED 硬件初始化
  OLED_Init();
  
  // 2. 清空显存
  OLED_Clear();
  
  // 3. 显示测试内容（仅操作内存显存）
  OLED_ShowString(0, 0, "STM32 OLED", OLED_8X16);  // 第0行：标题
  OLED_ShowString(0, 2, "Year:", OLED_8X16);       // 第2行：标签
  OLED_ShowNum(40, 2, 2026, 4, OLED_8X16);     // 第2行：数字
  
  OLED_DrawLine(0, 20, 127, 60); // 画一条斜线
  OLED_DrawRectangle(100, 0, 27, 16, OLED_UNFILLED); // 画一个方框

  // 4. 刷新显存到屏幕（必须调用！否则屏幕无显示）
  OLED_Update();

  // 任务主循环
  for(;;)
  {
    // 屏幕整体反显，验证周期刷新正常
    OLED_Reverse();
    OLED_Update();
    osDelay(1000);
    
    OLED_Reverse();
    OLED_Update();
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  红灯任务：PA0输出，亮1000ms，灭1000ms
  */
void redTask(void const * argument)
{
  for(;;)
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // 红灯亮
    osDelay(1000);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // 红灯灭
    osDelay(1000);
  }
}

/**
  * @brief  绿灯任务：PA1输出，亮300ms，灭1500ms
  */
void greenTask(void const * argument)
{
  for(;;)
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // 绿灯亮
    osDelay(300);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // 绿灯灭
    osDelay(1500);
  }
}
/* USER CODE END Application */


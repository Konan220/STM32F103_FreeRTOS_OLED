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
#include "Key.h"
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
void keyTask(void const * argument);
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

  /* 按键扫描任务 */
  osThreadDef(keyTask, keyTask, osPriorityNormal, 0, 256);
  osThreadCreate(osThread(keyTask), NULL);
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
  for(;;)
  {
    // 空闲循环
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
  * @brief  在OLED上显示带符号的数字，格式：+xxx / -xxx / 000
  * @param  x, y: 显示起始坐标（左上角）
  * @param  num: 要显示的数，范围 -200 ~ 200
  * @note   手动拆位显示，不依赖 sprintf，避免嵌入式库格式化异常
  *         每个字符宽 8 像素（OLED_8X16）
  */
static void OLED_ShowSignedNumber(uint8_t x, uint8_t y, int16_t num)
{
    uint16_t abs_num;
    char sign;

    // 先清空 4 个字符位置（符号位+3位数字），防止残影
    OLED_ShowString(x, y, "    ", OLED_8X16);

    if (num > 0) {
        sign = '+';
        abs_num = (uint16_t)num;
    } else if (num < 0) {
        sign = '-';
        abs_num = (uint16_t)(-num);
    } else {
        // 0 显示为 000（不带符号）
        OLED_ShowChar(x,      y, '0', OLED_8X16);
        OLED_ShowChar(x + 8,  y, '0', OLED_8X16);
        OLED_ShowChar(x + 16, y, '0', OLED_8X16);
        return;
    }

    // 显示符号位
    OLED_ShowChar(x,      y, sign, OLED_8X16);
    // 显示三位数字（百位 / 十位 / 个位）
    OLED_ShowChar(x + 8,  y, '0' + (abs_num / 100) % 10, OLED_8X16);
    OLED_ShowChar(x + 16, y, '0' + (abs_num / 10)  % 10, OLED_8X16);
    OLED_ShowChar(x + 24, y, '0' + (abs_num)       % 10, OLED_8X16);
}

/**
  * @brief  按键任务：扫描按键+OLED显示数值，KEY1加1，KEY2减1
  * @note   数值范围 -200 ~ 200，显示格式：+xxx / -xxx / 000
  */
/**
  * @brief  按键任务：接收中断事件 + OLED显示数值
  * @note   数值范围 -200 ~ 200，显示格式：+xxx / -xxx / 000
  */
void keyTask(void const * argument)
{
    int16_t num = 0;     // 计数变量，范围 -200 ~ 200

    Key_Init();          // 初始化按键消息队列
    OLED_Init();         // 初始化OLED屏幕
    OLED_Clear();        // 清屏

    // 初始显示：第一行显示标签和数值 000
    OLED_ShowString(0, 0, "Num:", OLED_8X16);
    OLED_ShowSignedNumber(32, 0, num);
    OLED_Update();       // 刷新到物理屏幕

    for(;;)
    {
        /* 1. 阻塞等待按键中断投递的消息，不再轮询 */
        KeyID key = Key_GetEvent(osWaitForever);
        if (key == KEY_NUM) {
            continue;    // 超时或无效事件
        }

        GPIO_TypeDef *port = (key == KEY_1) ? KEY1_PORT : KEY2_PORT;
        uint16_t      pin  = (key == KEY_1) ? KEY1_PIN  : KEY2_PIN;

        /* 2. 软件去抖：延时20ms后仍为低电平才确认有效按下 */
        osDelay(20);
        if (HAL_GPIO_ReadPin(port, pin) != GPIO_PIN_RESET) {
            continue;    // 已经松开，是抖动，丢弃
        }

        /* 3. 处理按键逻辑 */
        if (key == KEY_1) {
            if (num < 200) {
                num++;   // KEY1加1，上限200
            }
        } else if (key == KEY_2) {
            if (num > -200) {
                num--;   // KEY2减1，下限-200
            }
        }

        /* 4. 更新OLED显示 */
        OLED_ShowSignedNumber(32, 0, num);
        OLED_Update();

        /* 5. 等待按键释放，避免长按连续触发 */
        while (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) {
            osDelay(20);
        }
    }
}
/* USER CODE END Application */


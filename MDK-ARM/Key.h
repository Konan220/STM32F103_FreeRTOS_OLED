#ifndef __KEY_H
#define __KEY_H

#include "main.h"
#include "cmsis_os.h"

/**************** 引脚定义 ****************/
#define KEY1_PORT   GPIOA
#define KEY1_PIN    GPIO_PIN_2

#define KEY2_PORT   GPIOA
#define KEY2_PIN    GPIO_PIN_4

/**************** 状态定义 ****************/
typedef enum {
    KEY_1 = 0,  // 对应PA2
    KEY_2,      // 对应PA4
    KEY_NUM     // 按键总数
} KeyID;

/**************** 函数声明 ****************/
void Key_Init(void);
KeyID Key_GetEvent(uint32_t timeout);

#endif
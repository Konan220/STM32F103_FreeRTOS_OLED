#include "Key.h"

static osMessageQId keyQueueHandle;
osMessageQDef(keyQueue, 8, uint32_t);

void Key_Init(void)
{
    keyQueueHandle = osMessageCreate(osMessageQ(keyQueue), NULL);
}

KeyID Key_GetEvent(uint32_t timeout)
{
    osEvent evt = osMessageGet(keyQueueHandle, timeout);
    if (evt.status == osEventMessage) {
        return (KeyID)evt.value.v;
    }
    return KEY_NUM;
}

/* 中断回调函数，所有 EXTI 中断最终都会进这里 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t key_id = KEY_NUM;

    if (GPIO_Pin == KEY1_PIN) {
        key_id = KEY_1;
    } else if (GPIO_Pin == KEY2_PIN) {
        key_id = KEY_2;
    }

    if (key_id < KEY_NUM) {
        /* 在中断中发送消息到队列，内部会自动使用 xQueueSendFromISR */
        osMessagePut(keyQueueHandle, key_id, 0);
    }
}
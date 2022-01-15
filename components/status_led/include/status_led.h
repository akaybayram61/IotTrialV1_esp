#ifndef __STATUS_LED_H__
#define __STATUS_LED_H__

#define LED_HIGH 1
#define LED_LOW 0
#define ONE_SEC (1000 / portTICK_RATE_MS)
#define HB_BUMP_DELAY 150
#define HB_BUMP_COUNT 2
#define BL_DELAY 1000

#include "driver/gpio.h"

typedef enum{
    BLINK,
    HEARTBEAT
}led_mode_t;

enum{
    STATUS_LED_OK,
    STATUS_LED_ERR_INIT_FAILED,
    STATUS_LED_ERR_TASK_FAILED,
    STATUS_LED_ERR_UNINITIALIZED,
    STATUS_LED_ERR_INVALID_MODE,
};

void heartbeat();
void blink();

int32_t status_led_task_status_set(bool mode);
int32_t status_led_init(gpio_num_t, led_mode_t);
int32_t status_led_task();

#endif // __STATUS_LED_H__
#ifndef __STATUS_LED_H__
#define __STATUS_LED_H__

#define LED_HIGH 1
#define LED_LOW 0
#define ONE_SEC (1000 / portTICK_PERIOD_MS)
#define HB_BUMP_DELAY 150
#define HB_BUMP_COUNT 2
#define BL_DELAY 1000

#include "driver/gpio.h"

typedef enum{
    BLINK,
    HEARTBEAT
}led_mode_t;

extern const char *led_mode[];
extern gpio_num_t st_led_pin;
extern led_mode_t st_led_mode;
extern void (*led_funcs[2])();

void heartbeat();
void blink();

void status_led_init(gpio_num_t, led_mode_t);
void status_led_task();

#endif // __STATUS_LED_H__
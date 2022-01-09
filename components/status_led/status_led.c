#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "status_led.h"

#define TAG "status_led"

static const char *led_mode[] = {
    [BLINK] = "blink",
    [HEARTBEAT] = "heartbeat"
};

static void (*led_funcs[2])() = {
    [BLINK] = blink,
    [HEARTBEAT] = heartbeat
};


static gpio_num_t st_led_pin;
static led_mode_t st_led_mode;

void status_led_init(gpio_num_t pin, led_mode_t mode){
    st_led_mode = mode;
    st_led_pin = pin;
    gpio_set_direction(st_led_pin, GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG, "led init with mode %s", led_mode[st_led_mode]);
}

void status_led_task(){
    while (1){
        led_funcs[st_led_mode]();
    }
}

void heartbeat(){
    for(int i = 0; i < HB_BUMP_COUNT; ++i){
        gpio_set_level(st_led_pin, LED_HIGH);
        vTaskDelay(HB_BUMP_DELAY / portTICK_RATE_MS);
        gpio_set_level(st_led_pin, LED_LOW);
        vTaskDelay(HB_BUMP_DELAY / portTICK_RATE_MS);
    }
    vTaskDelay(ONE_SEC);
}

void blink(){
    gpio_set_level(st_led_pin, LED_HIGH);
    vTaskDelay(BL_DELAY / portTICK_RATE_MS);
    gpio_set_level(st_led_pin, LED_LOW);
    vTaskDelay(BL_DELAY / portTICK_RATE_MS);
}
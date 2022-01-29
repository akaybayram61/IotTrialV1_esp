#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
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

static gpio_num_t st_led_pin = -1;
static led_mode_t st_led_mode = -1;
volatile bool led_task_status = true;


int32_t status_led_task_status_set(bool status){
    led_task_status = status;
    return led_task_status;
}

int32_t status_led_init(gpio_num_t pin, led_mode_t mode){
    if(mode >= sizeof led_funcs){
        return STATUS_LED_ERR_INVALID_MODE;
    }
    st_led_mode = mode;
    st_led_pin = pin;
    int32_t status = gpio_set_direction(st_led_pin, GPIO_MODE_OUTPUT);
    if(status == ESP_OK){
        ESP_LOGI(TAG, "led init with mode %s", led_mode[st_led_mode]);
        return STATUS_LED_OK;
    }

    return STATUS_LED_ERR_INIT_FAILED;
}

int32_t status_led_task(){
    if(st_led_mode == -1 || st_led_pin == -1){
        return STATUS_LED_ERR_UNINITIALIZED;
    }

    while (led_task_status){
        led_funcs[st_led_mode]();
    }

    return STATUS_LED_OK;
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

#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "status_led.h"
#include "dht11.h"

#include "wifi_setup.h"
#include "server_data_update.h"
#include "http_server_tasks.h"

void app_main(){
    const char *TAG = "main";
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    // xTaskCreate(status_led_task, "status led", 1024, NULL, 0, NULL);

    // wifi ap and sta mode start
    iottrialv1_wifi_setup_components();
    // iottrialv1_wifi_init_sta();
    iottrialv1_wifi_init_softap();

    xTaskCreate(iottrialv1_http_server, "iottrialv1 http server", 4096, NULL, 1, NULL);    
    xTaskCreate(iottrialv1_measure_temp_and_humi, "iottrialv1 temp humi process", 2048, NULL, 0, NULL);
}

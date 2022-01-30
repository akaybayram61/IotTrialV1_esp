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
    //xTaskCreate(status_led_task, "status led", 1024, NULL, 0, NULL);

    // wifi ap and sta mode start
    wifi_setup_components();
    wifi_init_sta();
    wifi_init_softap();

    xTaskCreate(http_server, "http server", 1024, NULL, 1, NULL);    
    xTaskCreate(measure_temp_and_humi, "temp humi process", 2048, NULL, 0, NULL);
}

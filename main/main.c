/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#define LED_BUILTIN GPIO_NUM_5
#define LED_HIGH 1
#define LED_LOW 0
#define ONE_SEC (1000 / portTICK_PERIOD_MS)
#define BUMP_DELAY 150
#define BUMP_COUNT 2

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS
        },
    };

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

static void heartbeat(){
    gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
    ESP_LOGI("Led mode", "heartbeat");
    while (1)
    {
        for(int i = 0; i < BUMP_COUNT; ++i){
            gpio_set_level(LED_BUILTIN, LED_HIGH);
            vTaskDelay(BUMP_DELAY / portTICK_PERIOD_MS);
            gpio_set_level(LED_BUILTIN, LED_LOW);
            vTaskDelay(BUMP_DELAY / portTICK_PERIOD_MS);
        }
        vTaskDelay(BUMP_DELAY);
    }
}

void network_proc(){
    const char *TAG = "network_proc";
    const char *msg_template = "POST / HTTP/1.0\r\n"
    "Connection: keep-alive\r\n"
    "Content-type: text/json\r\n"
    "\r\n"
    "{\r\n"
    "   temp: %d;\r\n"
    "   humi: %d;\r\n"
    "};\r\n\r\n";

    char msg[256] = {0};
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    inet_aton("192.168.1.14", &addr.sin_addr);
    addr.sin_port = htons(8000);
 
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        ESP_LOGE(TAG, "Socket create error invalid fd: %d", sock_fd);
        ESP_LOGE(TAG, "%s", strerror(errno));
        vTaskDelete(NULL);
    } 
    
    int conn_stat = connect(sock_fd, (struct sockaddr *)&addr, sizeof addr);
    if(conn_stat != 0){
        ESP_LOGE(TAG, "Connection error (return code: %d)", conn_stat);
        ESP_LOGE(TAG, "%s", strerror(errno));
        ESP_LOGI(TAG, "Task deleted...");
        vTaskDelete(NULL);
    }
    sprintf(msg, msg_template, 10, 10);
    size_t msg_len = strlen(msg);
    size_t nbyte = send(sock_fd, msg, msg_len, 0);
    if(nbyte != msg_len){
        ESP_LOGE(TAG, "Send error (only %d/%d byte sended.)", nbyte, msg_len);
        ESP_LOGE(TAG, "%s", strerror(errno));
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Process completed...");
    printf("%s\n", msg);
    vTaskDelay(ONE_SEC);
    close(sock_fd);
    vTaskDelete(NULL);
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    xTaskCreate(heartbeat, "led_heartbeat", 1024, NULL, 0, NULL);
    wifi_init_sta();
    xTaskCreate(network_proc, "network process", 1024, NULL, 0, NULL);
}

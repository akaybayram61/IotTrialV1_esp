#ifndef __WIFI_SETUP__
#define __WIFI_SETUP__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "string.h"

void iottrialv1_wifi_setup_components();
void iottrialv1_wifi_init_sta();
void iottrialv1_wifi_init_softap();
#endif //__WIFI_SETUP__

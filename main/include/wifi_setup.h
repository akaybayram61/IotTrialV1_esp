#ifndef _WIFI_SETUP_
#define _WIFI_SETUP_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "string.h"

void wifi_setup_components();
void wifi_init_sta();
void wifi_init_softap();
#endif // _WIFI_SETUP_
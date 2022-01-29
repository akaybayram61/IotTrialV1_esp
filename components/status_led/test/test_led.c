#include "status_led.h"
#include <unity.h>
const char *MODULE = "[status_led]";

void status_led_tests(void){
    UNITY_BEGIN();
    TEST_CASE("status_led_task uninitialized", MODULE {
        int status = status_led_task();
        TEST_ASSERT(status == STATUS_LED_ERR_UNINITIALIZED);    
    }
    
    TEST_CASE("status_led_init invalid mode", MODULE {
        int status = status_led_init(GPIO_NUM_4, LED_MODE_SIZE + 1);
        TEST_ASSERT(status == STATUS_LED_ERR_INVALID_MODE);
    }
    
    TEST_CASE("status_led_init init failed", MODULE {
        int status = status_led_init(17, HEARTBEAT);
        TEST_ASSERT(status == STATUS_LED_ERR_INIT_FAILED);
    }
    
    TEST_CASE("status_led_init success", MODULE {
        int status = status_led_init(GPIO_NUM_4, HEARTBEAT);
        TEST_ASSERT(status == STATUS_LED_OK);
    }
    
    TEST_CASE("status_led_task success", MODULE {
        status_led_task_status_set(false);
        int status = status_led_task();
        TEST_ASSERT(status == STATUS_LED_OK);
    }
    
    TEST_CASE("status_led_task_status_set", MODULE {
        bool val = false;
        bool status = status_led_task_status_set(val);
        TEST_ASSERT(status == val);
    }
    UNITY_END();
}
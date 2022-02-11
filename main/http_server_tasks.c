#include "http_server_tasks.h"
#include "mini_http.h"

static const char *greet_msg = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: 109\r\n\
Connection: Close\r\n\
\r\n\
<html><p id=\"demo\"></p><script>document.getElementById(\"demo\").innerHTML=\"Hello JavaScript!\";</script></html>\r\n\r\n";

static void greet_pair(int s){
    size_t size = send(s, greet_msg, strlen(greet_msg), 0);
    if(size != strlen(greet_msg))
        ESP_LOGE("greet_pair", "Failed to send greet message!");
}

void iottrialv1_http_server(){
    const char *TAG = "http_server";
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        ESP_LOGE(TAG, "Socket error!");
        vTaskDelete(NULL);
    }
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80); 
    
    int bind_st = bind(fd, (struct sockaddr *)&addr, sizeof addr);
    if(bind_st == -1){
        ESP_LOGE(TAG, "Bind Error!");
        vTaskDelete(NULL);
    }
    
    int lis_st = listen(fd, 1);
    if(lis_st == -1){
        ESP_LOGE(TAG, "Listen error!");
        vTaskDelete(NULL);
    }
    
    int pair_fd = 0; 
    struct sockaddr_in pair_addr = {0};
    socklen_t pair_len = sizeof pair_addr;
    ESP_LOGI(TAG, "Waiting for pairs!");
    while ((pair_fd = accept(fd, (struct sockaddr *)&pair_addr, &pair_len)) != -1 ){
        greet_pair(pair_fd);
        ESP_LOGI(TAG, "Pair accepted and greeted!");
        close(pair_fd);
    }
}

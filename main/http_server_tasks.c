#include "http_server_tasks.h"
#include "mini_http.h"

static void greet_pair(int s){
    size_t size = 0;
    char sbuffer[1024] = {0};
    size = recv(s, sbuffer, sizeof sbuffer, 0);
    
    HTTPResp resp = {
        .version = HTTP_VER_1_1,
        .status_code = HTTP_OK,
        .connection = true,
        .content_type = "text/html",
    };
    resp.content_length = strlen(index_html);
    mini_http_gen_resp_str(&resp, sbuffer, sizeof sbuffer);
    ESP_LOGW("greet_pair", "%s", sbuffer);

    ESP_LOGW("greeter", "%s", index_html);
    size = send(s, sbuffer, strlen(sbuffer), 0);

    if(size != strlen(sbuffer))
        ESP_LOGE("greet_pair", "Failed to send greet message!");
    else
        ESP_LOGE("greet_pair", "header send succesfull!");

    size = send(s, index_html, strlen(index_html), 0);
    
    if(size != strlen(index_html))
        ESP_LOGE("greet_pair", "Failed to greeting!");
    else
        ESP_LOGE("greet_pair", "data send succesfull!");
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
    }
}

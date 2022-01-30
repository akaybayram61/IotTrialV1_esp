#include "esp_log.h"
#include "server_data_update.h"
#include "dht11.h"

#define DHT11_SENS GPIO_NUM_13

struct addrinfo *get_server_info(char *address, char *port){
    // API ip adresi bulma
    struct addrinfo hints;
    struct addrinfo *res;
    hints.ai_family = AF_INET;
    int ret = lwip_getaddrinfo("iottrial.herokuapp.com",
                     "80",
                     &hints,
                     &res);
    if (ret != 0){
        return NULL;
    }
    return res;
}

static void send_data_to_server(int temp, int humi, struct addrinfo *addr){
    const char *TAG = "network_proc";
    const char *msg_template = "POST /value HTTP/1.1\r\n"
    "Host: iottrial.herokuapp.com\r\n"
    "Content-type: application/json\r\n"
    "ApiKey: n0yxYYpLm2\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\r\n";
    const char *json_template = "{\"temperature\": %d,\"humidity\": %d}\r\n\r\n";
    char msg[512];
    memset(msg, 0, sizeof msg);
    
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        ESP_LOGE(TAG, "Socket create error (error code: %d)", errno);
        ESP_LOGE(TAG, "%s", strerror(errno));
        return;
    } 
    
    int conn_stat = connect(sock_fd, addr->ai_addr, addr->ai_addrlen);
    if(conn_stat != 0){
        ESP_LOGE(TAG, "Connection error (error code: %d)", errno);
        ESP_LOGE(TAG, "%s", strerror(errno));
        ESP_LOGI(TAG, "Task deleted...");
        return;
    }
    size_t nbyte, s;
    s = sprintf(msg, msg_template, strlen(json_template));
    sprintf(msg + s, json_template, temp, humi);
    size_t msg_len = strlen(msg);
    // Send value to server
    nbyte = send(sock_fd, msg, msg_len, 0);
    if(nbyte != msg_len){
        ESP_LOGE(TAG, "Send error (only %d/%d byte sended.)", nbyte, msg_len);
        ESP_LOGE(TAG, "%s", strerror(errno));
        return;
    }
    ESP_LOGI(TAG, "Post isteği gönderildi. Cevap bekleniyor."); 
    // Get answer from server
    memset(msg, 0, sizeof msg); 
    while (recv(sock_fd, msg, sizeof msg, 0) != 0){
        printf("%s", msg);
        if(strstr(msg, "\r\n\r\n") != NULL){
            break;
        }
    };
    printf("\n");
    ESP_LOGI(TAG, "Send process succeded...");
    close(sock_fd);
    return;
}

void measure_temp_and_humi(){
    const char *TAG = "Temparature and Humidity Task";
    struct addrinfo *server;
    int try_count = 5;
    while (try_count > 0){
        server = get_server_info("iottrial.herokuapp.com", "80");
        if(server == NULL){
            ESP_LOGE(TAG, "Server not found! Retrying...");
            try_count--;
            vTaskDelay(1000 / portTICK_RATE_MS);
        }else
            break;
    }

    if(server == NULL){
        ESP_LOGE(TAG, "Server not found!");
        vTaskDelete(NULL);
    }
    
    DHT11_init(GPIO_NUM_5);
    while (1){
        struct dht11_reading data; 
        data = DHT11_read(); 
        if(data.status == DHT11_OK){
            ESP_LOGI(TAG, "temp: %d, humi: %d", data.temperature, data.humidity);
            send_data_to_server(data.temperature, data.humidity, server);
            vTaskDelay(5000 / portTICK_RATE_MS);
        }
    }
    vTaskDelete(NULL); 
}

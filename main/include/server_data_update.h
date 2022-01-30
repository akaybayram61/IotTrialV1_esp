#ifndef _SERVER_DATA_UPDATE_H_
#define _SERVER_DATA_UPDATE_H_

#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_timer.h"

struct addrinfo *get_server_info(char *address, char *port);
void measure_temp_and_humi();
#endif // _SERVER_DATA_UPDATE_H_
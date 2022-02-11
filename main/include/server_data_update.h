#ifndef __SERVER_DATA_UPDATE_H__
#define __SERVER_DATA_UPDATE_H__

#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_timer.h"

struct addrinfo *iottrialv1_get_server_info(char *address, char *port);
void iottrialv1_measure_temp_and_humi();
#endif // _SERVER_DATA_UPDATE_H_

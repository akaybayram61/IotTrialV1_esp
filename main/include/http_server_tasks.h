#ifndef _HTTP_SERVER_TASKS_
#define _HTTP_SERVER_TASKS_

#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_timer.h"
#include "esp_log.h"

void greet_pair(int s);
void http_server();

#endif // _HTTP_SERVER_TASKS_
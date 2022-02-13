#ifndef _HTTP_SERVER_TASKS_
#define _HTTP_SERVER_TASKS_

#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "html_pages.h"
#include "mini_http.h"

#define MAX_RECV_BUFF_LEN 512

void iottrialv1_http_server();

#endif // _HTTP_SERVER_TASKS_

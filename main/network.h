#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define NET_SSID "testAP"
#define NET_PWD "12354678"

esp_err_t wifi_init();



#endif
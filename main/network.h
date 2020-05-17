#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h" 
#include "esp_http_client.h"

#define NET_SSID "lizi502"
#define NET_PWD "11028888"

#define ID_URL "http://192.168.0.103:5000/id"
#define UPLOAD_URL "http://192.168.0.103:5000/upload"
#define HTTP_PORT 5000

#define POST_LEN 80

#define TAG "shouhuan"

esp_err_t wifi_init();

esp_err_t http_upload(int heartRate,int bloodOxy,float temp);

#endif
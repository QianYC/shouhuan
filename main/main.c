/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "max30102.h"
#include "network.h"

void app_main(void){
    maxData data = {-1,-1,0.0};
   	max30102_init();
    esp_err_t wifi_err = wifi_init();
    for(;;){
        max30102_read(&data);
        printf("HR:%d,BO:%d,temp:%f\n",data.heartRate,data.bloodOxy,data.temp);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

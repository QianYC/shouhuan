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

#include "driver/i2c.h"

//pin definitions for MAX30102
#define SCL_PIN 26
#define SDA_PIN 25
#define MAX30102_ADDR 0x57

uint8_t buffer[32];

static void i2c_master_init(){
    i2c_config_t config;
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = SDA_PIN;
    config.scl_io_num = SCL_PIN;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void read_max30102(){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX30102_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buffer, sizeof(buffer), I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_RATE_MS);
}

void task(void* pvParameter){
    while(true){
        read_max30102();
        printf("read buffer...\n");
        for (size_t i = 0; i < sizeof(buffer); i++){
            printf("%d ", buffer[i]);
        }
        printf("\nbuffer ends...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void){
    i2c_master_init();
    xTaskCreate(task, "MAX30103", 1024 * 2, NULL, 1, NULL);
}

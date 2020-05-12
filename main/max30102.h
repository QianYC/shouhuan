#ifndef __MAX30102_H_
#define __MAX30102_H_

#include <stdio.h>
#include "driver/i2c.h"
#include "esp_system.h"
#include "sdkconfig.h"

//Registers
#define MAX_INT_STA_1_ADDR 0x00
#define MAX_INT_STA_2_ADDR 0x01
#define MAX_INT_EN_1_ADDR 0x02
#define MAX_INT_EN_2_ADDR 0x03

#define MAX_FIFO_WR_PTR_ADDR 0x04
#define MAX_OVF_CTR_ADDR 0x05
#define MAX_FIFO_RD_PTR_ADDR 0x06
#define MAX_FIFO_DATA_ADDR 0x07

#define MAX_FIFO_CONF_ADDR 0x08
#define MAX_MODE_CONF_ADDR 0x09
#define MAX_SPO2_CONF_ADDR 0x0A
#define MAX_LED1_ADDR 0x0C
#define MAX_LED2_ADDR 0x0D
#define MAX_MULT_LED_CONF1_ADDR 0x11
#define MAX_MULT_LED_CONF2_ADDR 0x12


#define MAX_DIE_TEMP_INT_ADDR 0x1F
#define MAX_DIE_TEMP_FRAC_ADDR 0x20
#define MAX_DIE_TEMP_CONF_ADDR 0x21

//I2C: Any GPIO Pin
#define MAX_I2C_SCL 25
#define MAX_I2C_SDA 26
#define MAX_I2C_NUM I2C_NUM_0
#define MAX_I2C_ADDR 0x57
#define MAX_I2C_FREQ_HZ (200*1000)

#define MAX_SAMPLE_MAX 32
#define MAX_BYTE_PER_SAMPLE 6
#define MAX_TEMP_FRAC_STEP 0.0625

//variable to store the last fifo read position
// uint8_t rd_pointer = 0;

typedef struct max_data_struct{
    int heartRate;
    int bloodOxy;
    float temp;
} maxData;

esp_err_t max30102_init();

esp_err_t max30102_read(maxData *data);

#endif

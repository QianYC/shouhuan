#ifndef __MAX30102_H_
#define __MAX30102_H_

#include <stdio.h>
#include "driver/i2c.h"
#include "sdkconfig.h"

//Registers
#define INT_STA_1_ADDR 0x00
#define INT_STA_2_ADDR 0x01
#define INT_EN_1_ADDR 0x02
#define INT_EN_2_ADDR 0x03

#define FIFO_WR_PTR_ADDR 0x04
#define OVF_CTR_ADDR 0x05
#define FIFO_RD_PTR_ADDR 0x06
#define FIFO_DATA_ADDR 0x07

#define FIFO_CONF_ADDR 0x08
#define MODE_CONF_ADDR 0x09
#define SPO2_CONF_ADDR 0x0A

#define DIE_TEMP_INT_ADDR 0x1F
#define DIE_TEMP_FRAC_ADDR 0x20
#define DIE_TEMP_CONF_ADDR 0x21

//I2C: Any GPIO Pin
#define MAX_I2C_SCL 25
#define MAX_I2C_SDA 26
#define MAX_I2C_NUM I2C_NUM_0
#define MAX_I2C_ADDR 0x57

#endif

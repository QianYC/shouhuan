#include "max30102.h"
#include "esp_log.h"

#define ENABLE 1
#define DISABLE 0
#define ACK_VAL 0x0
#define NACK_VAL 0x1

#define WRITE_BIT 0
#define READ_BIT 1

#define ERR_CHECK      \
    if (err != ESP_OK) \
    {                  \
        return err;    \
    }

static esp_err_t i2c_init()
{
    i2c_config_t c;
    c.mode = I2C_MODE_MASTER;
    c.sda_io_num = MAX_I2C_SDA;
    c.sda_pullup_en = GPIO_PULLUP_ENABLE;
    c.scl_io_num = MAX_I2C_SCL;
    c.scl_pullup_en = GPIO_PULLUP_ENABLE;
    c.master.clk_speed = MAX_I2C_FREQ_HZ;
    i2c_param_config(MAX_I2C_NUM, &c);
    return i2c_driver_install(MAX_I2C_NUM, c.mode, DISABLE, DISABLE, 0);
}

static esp_err_t i2c_read_reg(int reg, uint8_t *data_rd, size_t size)
{
    if (size == 0)
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX_I2C_ADDR << 1) | WRITE_BIT, ENABLE);
    i2c_master_write_byte(cmd, reg, ENABLE);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX_I2C_ADDR << 1) | READ_BIT, ENABLE);
    for (size_t i = 0; i < size; i++)
    {
        i2c_master_read_byte(cmd, data_rd + i - 1, NACK_VAL);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(MAX_I2C_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_write_byte_reg(int reg, uint8_t data_wr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX_I2C_ADDR << 1) | WRITE_BIT, ENABLE);
    i2c_master_write_byte(cmd, reg, ENABLE);

    i2c_master_write_byte(cmd, data_wr, ENABLE);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(MAX_I2C_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_read_fifo(uint8_t *fifo_data)
{
    esp_err_t err;
    static uint8_t rd_pointer = 0;
    uint8_t wr_pointer;
    err = i2c_read_reg(MAX_FIFO_WR_PTR_ADDR, &wr_pointer, 1);
    ERR_CHECK

    size_t byte2read = (wr_pointer - rd_pointer);

    err = i2c_read_reg(MAX_FIFO_DATA_ADDR, fifo_data, byte2read);
    return err;
}

static esp_err_t i2c_read_temp(uint8_t *temp_int, uint8_t *temp_frac)
{
    esp_err_t err;
    err = i2c_read_reg(MAX_DIE_TEMP_INT_ADDR, temp_int, 1);
    ERR_CHECK

    err = i2c_read_reg(MAX_DIE_TEMP_FRAC_ADDR, temp_frac, 1);
    return err;
}

static esp_err_t max_configure()
{
    esp_err_t err=ESP_OK;
    //TODO: configure reg
    return err;
}

esp_err_t max30102_init()
{
    esp_err_t err;
    err = i2c_init();
    ERR_CHECK

    err = max_configure();

    return 0;
}

esp_err_t max30102_read(maxData *data)
{
    esp_err_t err;
    uint8_t fifo_data[MAX_SAMPLE_MAX * MAX_BYTE_PER_SAMPLE];
    uint8_t temp_int, temp_frac;

    err = i2c_read_fifo(fifo_data);
    ERR_CHECK

    err = i2c_read_temp(&temp_int, &temp_frac);
    ERR_CHECK

    //TODO: get bloodOxy and heartRate

    //get temperature
    if ((temp_int & (1 << 7)) == 0)
    {
        data->temp = (float)(int)temp_int;
    }
    else
    { //temperature below zero, the person is probably dead
        data->temp = (float)((int)temp_int - (1 << 8));
    }
    data->temp += (float) temp_frac * MAX_TEMP_FRAC_STEP ;

    return err;
}
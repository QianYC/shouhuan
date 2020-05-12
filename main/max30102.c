#include "max30102.h"
#include "esp_log.h"
#include "spo2.h"

#define ENABLE 1
#define DISABLE 0
#define ACK_VAL 0x0
#define NACK_VAL 0x1

#define WRITE_BIT 0
#define READ_BIT 1

#define TIMEOUT_COUNT 20

//#define SMOOTH_FACTOR 0.08

#define ERR_CHECK                       \
    if (err != ESP_OK)                  \
    {                                   \
        printf("error code %x\n", err); \
        return err;                     \
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
    for (size_t i = 0; i < size - 1; i++)
    {
        i2c_master_read_byte(cmd, data_rd + i, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
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

static esp_err_t i2c_read_fifo(uint8_t *fifo_data, uint8_t *sample_count)
{
    esp_err_t err;
    static uint8_t rd_pointer = 0;
    uint8_t wr_pointer;
    err = i2c_read_reg(MAX_FIFO_WR_PTR_ADDR, &wr_pointer, 1);
    ERR_CHECK

    size_t sample2read = wr_pointer - rd_pointer;
    if ((int)sample2read < 0)
    {
        sample2read += MAX_SAMPLE_MAX;
    }
    *sample_count = sample2read;

    err = i2c_read_reg(MAX_FIFO_DATA_ADDR, fifo_data, sample2read * MAX_BYTE_PER_SAMPLE);
    ERR_CHECK

    rd_pointer = wr_pointer;
    return err;
}

static esp_err_t i2c_read_temp(uint8_t *temp_int, uint8_t *temp_frac)
{
    esp_err_t err;

    err = i2c_write_byte_reg(MAX_DIE_TEMP_CONF_ADDR, 0x01);
    ERR_CHECK

    err = i2c_read_reg(MAX_DIE_TEMP_INT_ADDR, temp_int, 1);
    ERR_CHECK

    err = i2c_read_reg(MAX_DIE_TEMP_FRAC_ADDR, temp_frac, 1);
    return err;
}

static esp_err_t max_configure()
{
    esp_err_t err;
    //Configure Interrupt: Disable all interrupts(for now)
    err = i2c_write_byte_reg(MAX_INT_EN_1_ADDR, 0x00);
    ERR_CHECK

    err = i2c_write_byte_reg(MAX_INT_EN_2_ADDR, 0x00);
    ERR_CHECK

    //Configure FIFO: 4 average, enable rollover(continue filling new data when full), almost full set to 32
    err = i2c_write_byte_reg(MAX_FIFO_CONF_ADDR, 0x50);
    ERR_CHECK

    //Configure Mode: SPO2 mode
    err = i2c_write_byte_reg(MAX_MODE_CONF_ADDR, 0x03);
    ERR_CHECK

    //Configure SPO2: Range 4096, 100 samples/sec, 411us pulse width(ADC resolution 18 bits)
    err = i2c_write_byte_reg(MAX_SPO2_CONF_ADDR, 0x27);
    ERR_CHECK

    //Configure LED Current: set a small current
    err = i2c_write_byte_reg(MAX_LED1_ADDR, 0x2A);
    ERR_CHECK

    err = i2c_write_byte_reg(MAX_LED2_ADDR, 0x2A);
    ERR_CHECK

    //Configure Multi-LED: SLOT1 IR, SLOT2 Red, SLOT3/4 Disabled
    err = i2c_write_byte_reg(MAX_MULT_LED_CONF1_ADDR, 0x12);
    ERR_CHECK

    err = i2c_write_byte_reg(MAX_MULT_LED_CONF2_ADDR, 0x0);
    ERR_CHECK

    return err;
}

esp_err_t max30102_init()
{
    esp_err_t err;
    err = i2c_init();
    ERR_CHECK

    err = max_configure();

    return err;
}
/* Not Good, Maybe Use FFT?
static bool isABeat(int redAmp)
{
    static float averageAmp;
    static float lowAmp;
    static float highAmp;
    static int state;

    averageAmp = redAmp * SMOOTH_FACTOR + averageAmp * (1 - SMOOTH_FACTOR);
    if (redAmp > averageAmp)
    {
        highAmp = redAmp * 2 * SMOOTH_FACTOR + highAmp * (1 - 2 * SMOOTH_FACTOR);
    }
    else
    {
        lowAmp = redAmp * 2 * SMOOTH_FACTOR + lowAmp * (1 - 2 * SMOOTH_FACTOR);
    }
    printf("Avg:%f,high:%f,low:%f\n",averageAmp,highAmp,lowAmp);
    switch(state){
        case 0://higher than high
            if(redAmp < lowAmp){
                state=1;
            }
        break; 
        case 1://lower than low
            if(redAmp > highAmp){
                state=0;
                return true;
            }
    }
    return false;
}
*/
esp_err_t max30102_read(maxData *data)
{
    esp_err_t err;
    uint8_t fifo_data[MAX_SAMPLE_MAX * MAX_BYTE_PER_SAMPLE];
    uint8_t temp_int, temp_frac;
    uint8_t samples = 0;
    static int RedBufPtr = 0, IRBufPtr = 0;
    static int BOTimeOutCount = 0,HRTimeOutCount = 0; 
    static uint32_t RedBuf[BUFFER_SIZE], IRBuf[BUFFER_SIZE];
    int32_t bloodOxy, heartRate;
    int8_t bloodOxyValid, heartRateValid;

    err = i2c_read_fifo(fifo_data, &samples);
    ERR_CHECK

    err = i2c_read_temp(&temp_int, &temp_frac);
    ERR_CHECK

    for (int i = 0; i < samples; i++)
    {
        IRBuf[IRBufPtr] = ((fifo_data[i * MAX_BYTE_PER_SAMPLE] << 16) + (fifo_data[i * MAX_BYTE_PER_SAMPLE + 1] << 8) + (fifo_data[i * MAX_BYTE_PER_SAMPLE + 2]));
        IRBufPtr = (IRBufPtr + 1) % BUFFER_SIZE;
        RedBuf[RedBufPtr] = ((fifo_data[i * MAX_BYTE_PER_SAMPLE + 3] << 16) + (fifo_data[i * MAX_BYTE_PER_SAMPLE + 4] << 8) + (fifo_data[i * MAX_BYTE_PER_SAMPLE + 5]));
        RedBufPtr = (RedBufPtr + 1) % BUFFER_SIZE;
    }
    maxim_heart_rate_and_oxygen_saturation(IRBuf, BUFFER_SIZE, RedBuf, &bloodOxy, &bloodOxyValid, &heartRate, &heartRateValid);
    if (bloodOxyValid)
    {
        data->bloodOxy = bloodOxy;
    }
    else{
        BOTimeOutCount++;
        if(BOTimeOutCount > TIMEOUT_COUNT){
            data->bloodOxy = -1;
        }
    }
    if (heartRateValid)
    {
        data->heartRate = heartRate;
    }
    else{
        HRTimeOutCount++;
        if(HRTimeOutCount > TIMEOUT_COUNT){
            data->heartRate = -1;
        }
    }
    //get temperature
    if ((temp_int & (1 << 7)) == 0)
    {
        data->temp = (float)(int)temp_int;
    }
    else
    { //temperature below zero, the person is probably dead
        data->temp = (float)((int)temp_int - (1 << 8));
    }
    data->temp += (float)temp_frac * MAX_TEMP_FRAC_STEP;

    return err;
}
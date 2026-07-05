#include "hal_bno086.h"
#include "driver/i2c_master.h" 
#include "driver/gpio.h"
#include "esp_err.h"


static i2c_master_bus_handle_t bus_handles[I2C_NUM_MAX] = {0};
static uint32_t saved_clk_speed = 400000; 

static hal_status_t map_esp_err(esp_err_t err) {
    switch (err) {
        case ESP_OK:           return HAL_OK;
        case ESP_ERR_TIMEOUT: return HAL_TIMEOUT;
        default:           return HAL_ERROR;
    }
}

hal_status_t I2CInit(uint8_t i2c_port, int sda_pin, int scl_pin, uint32_t clk_speed_hz) {
    if (i2c_port >= I2C_NUM_MAX) return HAL_ERROR;

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = (i2c_port_t)i2c_port,
        .scl_io_num = scl_pin,
        .sda_io_num = sda_pin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    saved_clk_speed = clk_speed_hz;

    esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handles[i2c_port]);
    return map_esp_err(err);
}

hal_status_t I2CWrite(uint8_t i2c_port, uint8_t dev_addr, const uint8_t *data, size_t len, uint32_t timeout_ms) {
    if (i2c_port >= I2C_NUM_MAX || bus_handles[i2c_port] == NULL) return HAL_ERROR;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = saved_clk_speed,
    };

    i2c_master_dev_handle_t dev_handle;
    if (i2c_master_bus_add_device(bus_handles[i2c_port], &dev_config, &dev_handle) != ESP_OK) {
        return HAL_ERROR;
    }

    
    esp_err_t err = i2c_master_transmit(dev_handle, data, len, timeout_ms);

    
    i2c_master_bus_rm_device(dev_handle);

    return map_esp_err(err);
}

hal_status_t I2CRead(uint8_t i2c_port, uint8_t dev_addr, uint8_t *data, size_t len, uint32_t timeout_ms) {
    if (i2c_port >= I2C_NUM_MAX || bus_handles[i2c_port] == NULL) return HAL_ERROR;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = saved_clk_speed,
    };

    i2c_master_dev_handle_t dev_handle;
    if (i2c_master_bus_add_device(bus_handles[i2c_port], &dev_config, &dev_handle) != ESP_OK) {
        return HAL_ERROR;
    }

    esp_err_t err = i2c_master_receive(dev_handle, data, len, timeout_ms);

    i2c_master_bus_rm_device(dev_handle);

    return map_esp_err(err);
}

hal_status_t GPIOConfigPin(int pin_num, hal_gpio_mode_t mode, bool pull_up) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin_num),
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE
    };

    io_conf.mode = (mode == HAL_GPIO_MODE_INPUT) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;

    return map_esp_err(gpio_config(&io_conf));
}

hal_status_t GPIOWritePin(int pin_num, bool level) {
    return map_esp_err(gpio_set_level((gpio_num_t)pin_num, level ? 1 : 0));
}

bool GPIOReadPin(int pin_num) {
    return gpio_get_level((gpio_num_t)pin_num) == 1;
}
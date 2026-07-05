#ifndef HAL_BNO086_H_
#define HAL_BNO086_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef enum {
    HAL_OK = 0,
    HAL_ERROR = -1,
    HAL_TIMEOUT = -2
} hal_status_t;


typedef enum {
    HAL_GPIO_MODE_INPUT,
    HAL_GPIO_MODE_OUTPUT
} hal_gpio_mode_t;

/**
 * @brief Inicializa el I2C del microcontrolador en modo Master.
 * @param i2c_port Puerto I2C a usar (ej: 0 o 1)
 * @param sda_pin Número de pin para SDA
 * @param scl_pin Número de pin para SCL
 * @param clk_speed_hz Velocidad del bus en Hz (ej: 400000 para Fast Mode)
 * @return HAL_OK si fue exitoso, HAL_ERROR en caso contrario.
 */
hal_status_t I2CInit(uint8_t i2c_port, int sda_pin, int scl_pin, uint32_t clk_speed_hz);

/**
 * @brief Realiza una escritura síncrona en el bus I2C.
 * @param i2c_port Puerto I2C utilizado
 * @param dev_addr Dirección I2C del dispositivo esclavo (BNO086)
 * @param data Puntero al buffer de datos a escribir
 * @param len Cantidad de bytes a escribir
 * @param timeout_ms Tiempo máximo de espera en milisegundos
 * @return hal_status_t 
 */
hal_status_t I2CWrite(uint8_t i2c_port, uint8_t dev_addr, const uint8_t *data, size_t len, uint32_t timeout_ms);

/**
 * @brief Realiza una lectura síncrona desde el bus I2C.
 * @param i2c_port Puerto I2C utilizado
 * @param dev_addr Dirección I2C del dispositivo esclavo (BNO086)
 * @param data Puntero al buffer donde se guardarán los datos leídos
 * @param len Cantidad de bytes a leer
 * @param timeout_ms Tiempo máximo de espera en milisegundos
 * @return hal_status_t 
 */
hal_status_t I2CRead(uint8_t i2c_port, uint8_t dev_addr, uint8_t *data, size_t len, uint32_t timeout_ms);

/**
 * @brief Configura un pin GPIO interno como entrada o salida digital.
 * @param pin_num Número del pin GPIO
 * @param mode Modo de operación (INPUT u OUTPUT)
 * @param pull_up Habilitar resistencia de pull-up interna
 * @return hal_status_t 
 */
hal_status_t GPIOConfigPin(int pin_num, hal_gpio_mode_t mode, bool pull_up);

/**
 * @brief Modifica el estado de un pin GPIO configurado como salida.
 * @param pin_num Número del pin GPIO
 * @param level Estado a escribir (true = Alto, false = Bajo)
 * @return hal_status_t 
 */
hal_status_t GPIOWritePin(int pin_num, bool level);

/**
 * @brief Lee el estado actual de un pin GPIO configurado como entrada.
 * @param pin_num Número del pin GPIO (ej. pin INT del BNO086)
 * @return true si el pin está en ALTO, false si está en BAJO.
 */
bool GPIOReadPin(int pin_num);

#endif // HAL_BNO086_H_
#ifndef BNO086_H_
#define BNO086_H_

#include <stdint.h>
#include <stdbool.h>

// Direcciones I2C del sensor
#define BNO086_I2C_ADDR_DEF      0x4A  
#define BNO086_I2C_ADDR_ALT      0x4B  

// Tipos de retorno específicos del negocio/sensor, NO de la HAL
typedef enum {
    BNO086_OK = 0,
    BNO086_ERR_HW,      // Error de bus o GPIO
    BNO086_ERR_TIMEOUT, // El sensor no responde eléctricamente
    BNO086_ERR_PROTOCOL // Paquete SHTP corrupto o inválido
} bno086_status_t;

// Estructura de configuración que el Main rellenará, pero que el Driver procesará
typedef struct {
    uint8_t i2c_port;
    int sda_pin;
    int scl_pin;
    uint32_t clk_speed_hz;
    uint8_t dev_addr;
    int rst_pin;
    int int_pin;
} bno086_config_t;

typedef struct {
    float i;        // Eje X del cuaternión
    float j;        // Eje Y del cuaternión
    float k;        // Eje Z del cuaternión
    float real;     // Componente real (W)
    float accuracy; // Precisión estimada en radianes
    uint8_t status; // Fiabilidad (0=Unreliable, 1=Low, 2=Medium, 3=High)
} bno086_rv_t;

extern bno086_rv_t current_rotation_vector;
/**
 * @brief Inicializa por completo el hardware periférico y el sensor BNO086.
 * @param config Puntero a la estructura de configuración de pines y bus.
 */
bno086_status_t BNO086_Init(const bno086_config_t *config);

/**
 * @brief Comprueba si el sensor tiene datos listos para ser leídos (revisa el pin INT).
 * @param config Puntero a la estructura de configuración.
 * @return true si hay datos listos, false en caso contrario.
 */


// --- Prototipos de las funciones principales ---
bno086_status_t BNO086_Init(const bno086_config_t *config);
bool BNO086_IsDataReady(const bno086_config_t *config);
bno086_status_t BNO086_ReadRawPacket(const bno086_config_t *config, uint8_t *buffer, uint16_t max_len);
bno086_status_t BNO086_EnableRotationVector(const bno086_config_t *config, uint32_t interval_us);
uint8_t BNO086_ReadAndProcessPacket(const bno086_config_t *config);

#endif // BNO086_H_
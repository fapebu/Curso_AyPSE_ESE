#include "bno086.h"
#include "hal_bno086.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h> // Para los printf de diagnóstico
#include "esp_log.h"
#include "bno086_define.h"
#define BNO086_WELCOME_BUF_SIZE  272
#define BNO086_BOOT_TIMEOUT_MS   3000

static uint8_t sequence_channel2 = 0;
bno086_rv_t current_rotation_vector; // Estructura global actualizada automáticamente

bno086_status_t BNO086_Init(const bno086_config_t *config) {
    static bool hw_initialized = false;

    if (!hw_initialized) {
        printf("I2CInit\n");

        if (I2CInit(config->i2c_port, config->sda_pin, config->scl_pin, config->clk_speed_hz) != HAL_OK) {
            return BNO086_ERR_HW;
        }

        printf("Configurando GPIOs (RST:%d, INT:%d)...\n", config->rst_pin, config->int_pin);
        if (GPIOConfigPin(config->rst_pin, HAL_GPIO_MODE_OUTPUT, false) != HAL_OK) return BNO086_ERR_HW;
        if (GPIOConfigPin(config->int_pin, HAL_GPIO_MODE_INPUT, true) != HAL_OK) return BNO086_ERR_HW;
        
        hw_initialized = true; 
    } else {
        printf("Hardware I2C ya inicializado previamente \n");
    }

    // Estado inicial en alto
    GPIOWritePin(config->rst_pin, true);
    vTaskDelay(pdMS_TO_TICKS(20));
    

    // Ejecutar RESET físico
    printf("Reiniciamos el dispositivo \n");
    
    GPIOWritePin(config->rst_pin, false);
    vTaskDelay(pdMS_TO_TICKS(2000)); // Esperamos 2 segundos con el pin de reset en bajo, espera necesaria.
    
    
    GPIOWritePin(config->rst_pin, true);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Espera necesaria antes de continuar luego del reset.

    // Esperar la interrupción SHTP
    
    uint32_t timeout_counter = BNO086_BOOT_TIMEOUT_MS;
    while (GPIOReadPin(config->int_pin) == true) { //leemos el pin de interrupcion para leer el mensaje de bienvenida del sensor.
        vTaskDelay(pdMS_TO_TICKS(1));
        timeout_counter--;
        
        // Cada 200ms imprimimos el estado para auditar
        if (timeout_counter % 200 == 0) {
            printf("esperando INT. Estado actual: %d. Tiempo restante: %lums\n",GPIOReadPin(config->int_pin), timeout_counter);
        }

        if (timeout_counter == 0) {
            printf("El pin INT nunca bajó tras %d ms.\n", BNO086_BOOT_TIMEOUT_MS);
            return BNO086_ERR_TIMEOUT; 
        }
    }
    
    printf("Procediendo a leer por I2C\n");

    uint8_t buffer[BNO086_WELCOME_BUF_SIZE] = {0};
    if (I2CRead(config->i2c_port, config->dev_addr, buffer, BNO086_WELCOME_BUF_SIZE, 200) != HAL_OK) {
        printf("Error al leer bus I2C en la dirección 0x%X\n", config->dev_addr);
        return BNO086_ERR_HW;
    }

    uint16_t packet_len = ((buffer[1] & 0x7F) << 8) | buffer[0];
    //printf("Longitud del paquete SHTP recibido: %d bytes\n", packet_len);
    
    if (packet_len < 4) {
        return BNO086_ERR_PROTOCOL; 
    }

    return BNO086_OK;
}

bno086_status_t BNO086_ReadRawPacket(const bno086_config_t *config, uint8_t *buffer, uint16_t max_len) {
    if (I2CRead(config->i2c_port, config->dev_addr, buffer, max_len, 100) != HAL_OK) {
        return BNO086_ERR_HW;
    }
    return BNO086_OK;
}
bool BNO086_IsDataReady(const bno086_config_t *config) { //Funcion para chequear la interrupcion de datos disponibles.
    return (GPIOReadPin(config->int_pin) == false);
}

bno086_status_t BNO086_TransmitPacket(const bno086_config_t *config, uint8_t channel, uint8_t *payload, uint8_t payload_len) {
    uint16_t total_length = payload_len + 4;
    uint8_t tx_buffer[64];

    if (total_length > sizeof(tx_buffer)) return BNO086_ERR_PROTOCOL;

    //Construir Cabecera SHTP (4 bytes)
    tx_buffer[0] = (uint8_t)(total_length & 0xFF);        // Length LSB
    tx_buffer[1] = (uint8_t)((total_length >> 8) & 0xFF); // Length MSB
    tx_buffer[2] = channel;                               // Channel
    tx_buffer[3] = sequence_channel2++;                   // Sequence Number

    //Copiar Payload
    for (uint8_t i = 0; i < payload_len; i++) {
        tx_buffer[4 + i] = payload[i];
    }

    //Enviar por I2C
    if (I2CWrite(config->i2c_port, config->dev_addr, tx_buffer, total_length, 100) != HAL_OK) {
        return BNO086_ERR_HW;
    }

    return BNO086_OK;
}

bno086_status_t BNO086_SetFeatureCommand(const bno086_config_t *config, uint8_t report_id, uint32_t interval_us, uint32_t sensitivity) {
    uint8_t payload[17] = {0}; 

    payload[0] = SHTP_REPORT_SET_FEATURE; 
    payload[1] = report_id; 
    payload[2] = 0; // Flags 

    // Sensibilidad (Change sensitivity)
    payload[3] = (uint8_t)(sensitivity & 0xFF);
    payload[4] = (uint8_t)((sensitivity >> 8) & 0xFF);

    // Intervalo de reporte (Report Interval) - Little Endian
    payload[5] = (uint8_t)(interval_us & 0xFF);
    payload[6] = (uint8_t)((interval_us >> 8) & 0xFF);
    payload[7] = (uint8_t)((interval_us >> 16) & 0xFF);
    payload[8] = (uint8_t)((interval_us >> 24) & 0xFF);

    // Batch Interval (0 = Desactivado)
    payload[9] = 0; payload[10] = 0; payload[11] = 0; payload[12] = 0;

    // Sensor Specific configuration
    payload[13] = 0; payload[14] = 0; payload[15] = 0; payload[16] = 0;

    return BNO086_TransmitPacket(config, SHTP_CHANNEL_CONTROL, payload, 17);
}

// Función específica para habilitar el Vector de Rotación
bno086_status_t BNO086_EnableRotationVector(const bno086_config_t *config, uint32_t interval_us) {
    return BNO086_SetFeatureCommand(config, SENSOR_REPORTID_GAME_ROTATION_VECTOR, interval_us, 0);
}



uint8_t BNO086_ReadAndProcessPacket(const bno086_config_t *config) {
    // 1. LECTURA ÚNICA Y CONTINUA
    // Leemos 128 bytes de un solo golpe. El ESP32 envía 1 START y 1 STOP al final.
    // Esto evita romper el paquete a la mitad y colgar el bus I2C.
    uint8_t full_packet[128] = {0};
    
    if (I2CRead(config->i2c_port, config->dev_addr, full_packet, 128, 50) != HAL_OK) {
        ESP_LOGE("ERROR:","Fallo al leer I2C (Bus bloqueado)");
        return 0;
    }

    //Extraer cabecera
    uint16_t packet_len = ((full_packet[1] & 0x7F) << 8) | full_packet[0];
    uint8_t channel = full_packet[2];
    
    //ESP_LOGI("Leido -> Len: %d, Canal: %d, Seq: %d, ID: 0x%02X",packet_len, channel, full_packet[3], full_packet[4]);

    if (packet_len < 4) return 0;
    
    // Protegemos nuestro límite de validación
    uint16_t valid_len = packet_len;
    if (valid_len > 128) valid_len = 128; 

    
    if (channel == SHTP_CHANNEL_INPUT) { //chequeamos que es un reporte valido
        uint16_t cursor = 4;
        uint8_t data_found_code = 0;

        while (cursor < valid_len) {
            uint8_t report_id = full_packet[cursor]; //leemos que sensor nos mando el dato.
            
            if (report_id == SHTP_REPORT_BASE_TIMESTAMP) { 
                cursor += 5; // Saltamos el timestamp
            } 
            else if (report_id == 0x05 || report_id == 0x08) { //si es vector de rotacion o game rotation.
                if ((cursor + 12) <= valid_len) {
                    current_rotation_vector.status = full_packet[cursor + 2] & 0x03;
                    //Tomamos los datos enviados
                    int16_t i_raw = (full_packet[cursor + 5] << 8) | full_packet[cursor + 4]; 
                    int16_t j_raw = (full_packet[cursor + 7] << 8) | full_packet[cursor + 6];
                    int16_t k_raw = (full_packet[cursor + 9] << 8) | full_packet[cursor + 8];
                    int16_t r_raw = (full_packet[cursor + 11] << 8) | full_packet[cursor + 10];
                    
                    //aplicamos el factor para escalarlos como corresponde.
                    float scale_q14 = 1.0f / 16384.0f;
                    current_rotation_vector.i = i_raw * scale_q14;
                    current_rotation_vector.j = j_raw * scale_q14;
                    current_rotation_vector.k = k_raw * scale_q14;
                    current_rotation_vector.real = r_raw * scale_q14;

                    if (report_id == 0x05 && (cursor + 14) <= valid_len) {
                        int16_t a_raw = (full_packet[cursor + 13] << 8) | full_packet[cursor + 12];
                        current_rotation_vector.accuracy = a_raw * (1.0f / 4096.0f);
                        cursor += 14; 
                    } else {
                        current_rotation_vector.accuracy = 0.0f;
                        cursor += 12;
                    }

                    data_found_code = 1;
                    //ESP_LOGI("BNO_TRACE", "¡Exito parseando vector de rotacion!");
                } else {
                    break; 
                }
            }
            else {
                // Reporte desconocido, salimos para no leer basura
                break; 
            }
        }
        return data_found_code;
    }

    return 0;
}

/*void BNO086_Task(void *pvParameters) {
    bno086_config_t *config = (bno086_config_t *)pvParameters;
    
    TickType_t lastPacketTime = xTaskGetTickCount();
    
    // Habilitar el Vector de Rotación a 20ms (20000us) -> 50Hz
    if (BNO086_EnableRotationVector(config, 20000) != BNO086_OK) {
        ESP_LOGE("BNO086", "Error al configurar Rotation Vector");
    }

    while(1) {
        // Verificar si el sensor ha dejado de responder por 2 segundos (2000 ms)
        if ((xTaskGetTickCount() - lastPacketTime) > pdMS_TO_TICKS(2000)) {
            ESP_LOGW("BNO086", "Timeout. Intentando reinicializar...");
            BNO086_Init(config); 
            BNO086_EnableRotationVector(config, 20000);
            lastPacketTime = xTaskGetTickCount();
        }

        // Si el pin INT está en bajo, hay datos disponibles
        uint8_t loopLimit = 5;
        while (GPIOReadPin(config->int_pin) == false && loopLimit > 0) {
            if (BNO086_ReadAndProcessPacket(config) != 0) {
                lastPacketTime = xTaskGetTickCount();
                
                // Imprimir resultados: 
                // Status: 0=Unreliable, 1=Low, 2=Medium, 3=High
                ESP_LOGI("BNO086", "RV [W:%.3f X:%.3f Y:%.3f Z:%.3f] - Status: %d", 
                         current_rotation_vector.real,
                         current_rotation_vector.i,
                         current_rotation_vector.j,
                         current_rotation_vector.k,
                         current_rotation_vector.status);
            }
            loopLimit--;
        }

        // Ceder CPU a otras tareas 
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}*/
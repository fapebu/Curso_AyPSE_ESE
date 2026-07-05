
#ifndef BNO085_DEFINE_H
#define BNO085_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

// --- Canales SHTP ---
#define SHTP_CHANNEL_CONTROL          2
#define SHTP_CHANNEL_INPUT            3

// --- Report IDs (Solicitudes) ---
#define SHTP_REPORT_PRODUCT_ID_REQ    0xF9
#define SHTP_REPORT_SET_FEATURE       0xFD
#define SHTP_REPORT_COMMAND_REQ       0xF2

// --- Sensor Report IDs ---
#define SENSOR_REPORTID_ACCEL_LIN     0x04
#define SENSOR_REPORTID_GRAVITY       0x06
#define SENSOR_REPORTID_GYRO          0x02
    
// --- Report IDs (Respuestas) ---
#define SHTP_REPORT_COMMAND_RESPONSE  0xF1
#define SHTP_REPORT_PRODUCT_ID_RESP   0xF8
#define SHTP_REPORT_BASE_TIMESTAMP    0xFB
#define SHTP_REPORT_GET_FEATURE_RESP  0xFC

// --- IDs de Comandos y Subcomandos ---
#define CMD_TARE                      0x03
#define SUBCMD_SET_REORIENTATION      0x02

#define SENSOR_REPORTID_ROTATION_VECTOR 0x05 // ID para el Rotation Vector
#define SENSOR_REPORTID_GAME_ROTATION_VECTOR 0x08
// --- Estructura para almacenar el Quaternion ---


#ifdef __cplusplus
}
#endif

#endif /* BNO085_DEFINE_H */
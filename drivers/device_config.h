#ifndef H_DEVICE_CONFIG_H
#define H_DEVICE_CONFIG_H

#include <stdint.h>

// Arduino mega có 1 SPI
typedef struct {
    uint32_t clock_hz;     // Tốc độ SPI
    uint8_t  spi_mode;     // SPI_MODE0..3
    uint8_t  bit_order;    // MSBFIRST / LSBFIRST
    uint8_t  cs_pin;       // Select pin
} SPI_DeviceConfig;

// Arduino mega chỉ có 1 I2C
typedef struct {
    uint32_t clock_hz;  // tốc độ I2C
    uint8_t  address;   // địa chỉ I2C của device
} I2C_DeviceConfig;

// Arduino mega có 4 UART
typedef struct {
    HardwareSerial* serial; 
    uint32_t baud;
} UART_DeviceConfig;

#endif

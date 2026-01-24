#ifndef GPIO_H
#define GPIO_H

#include "stdint.h"
#include "avr/io.h"

// Defines mode for PIN
#define OUTPUT       0
#define INPUT        1
#define INPUT_PULLUP 2

// Defines the logic output to pin
#define HIGH   1
#define LOW    0

void GPIOx_ConfigMode(volatile uint8_t* DDRx, volatile uint8_t* PORTx, uint8_t PIN, bool MODE);
void GPIOx_Write(volatile uint8_t* PORTx, uint8_t PIN,  bool LOGIC);
void GPIOx_WriteToggle(volatile uint8_t* PORTx, uint8_t PIN);
bool GPIOx_Read(volatile uint8_t* PIN_REG, uint8_t PIN);

#endif

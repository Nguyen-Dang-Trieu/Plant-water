#ifndef UART_H
#define UART_H

#include "stdint.h"
#include "avr/io.h"

#define FOSC  16000000UL  // Clock Speed
#define BAUD  9600
#define MYUBRR ((FOSC/(16UL * BAUD)) - 1)

void USARTx_Init(uint16_t UBRR);
void USARTx_Transmit(uint8_t DATA);
void USARTx_Print(const char* string);
uint8_t UARTx_Receive(void);

#endif
#include "uart.h"

void USARTx_Init(uint16_t UBRR)
{
  /*Set baud rate */
  UBRR0H = (uint8_t)(UBRR >> 8);
  UBRR0L = (uint8_t)UBRR;

  /* Enable receiver and transmitter */
  UCSR0B = (1 << RXEN0) |(1 << TXEN0);

  /* Set frame format: 8data, 1stop bit */
  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);  // UCSZ00 = 1 và UCSZ01 = 1 => 8bit data
}

void USARTx_Transmit(uint8_t DATA)
{
  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0))); // Bit 5 - UDRE0 của thanh ghi UCSR01 = 1 => UDR0 trống, sẵn sàng nhận data.

  /* Put data into buffer, sends the data */
  UDR0 = DATA;
}

void USARTx_Print(const char* str)
{
  while(*str) {
    USARTx_Transmit(*str++);
  }
}

uint8_t USARTx_Receive(void)
{
  /* Wait for data to be received */
  while (!(UCSR0A & (1 << RXC0)));

  /* Get and return received data from buffer */
  return UDR0;
}
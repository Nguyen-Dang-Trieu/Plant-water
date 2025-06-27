#include "adc.h"

void ADCx_Init()
{
  //AVCC with external capacitor at AREF pin (Vref = 5V)
  ADMUX = (1 << REFS0);

  // Tắt digital input ở các chân ADC0–ADC5 để giảm nhiễu
  DIDR0 = 0x3F; // Bit 5:0

  // Bật ADC, Prescaler = 128 → 16MHz / 128 = 125kHz (chuẩn)
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADCx_Read(uint8_t setChannelADC)
{
  // Setup channel ADC từ ADC0 -> ADC7
  setChannelADC &= 0x07;  // Only use 3-bit low of the ADMUX Register.

  // Select Channel ADC0
  ADMUX = (ADMUX & 0xF0) | setChannelADC;

  // Start converting
  ADCSRA |= (1 << ADSC);

  // Wait until the conversion is complete
  while (ADCSRA & (1 << ADSC));

  // Read 10-bit data
  uint16_t result = ADCL;           // Read 8-bit low
  result |= ((uint16_t)ADCH << 8);  // Read 2-bit high

  return result;
}
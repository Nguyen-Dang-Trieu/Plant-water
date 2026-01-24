#ifndef ADC_h
#define ADC_H

#include <avr/io.h>

void ADCx_Init();
uint16_t ADCx_Read(uint8_t setChannelADC);

#endif // ADC_h
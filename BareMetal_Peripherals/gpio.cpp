#include "gpio.h"

void GPIOx_ConfigMode(volatile uint8_t* DDRx, volatile uint8_t* PORTx, uint8_t PIN, bool MODE)
{
  switch(MODE) {
  case OUTPUT:
    *DDRx |= (1 << PIN);
    break;
  case INPUT:
    *DDRx &= ~(1 << PIN);
    *PORTx &= ~(1 << PIN); // Disable pull-up
    break;
  case INPUT_PULLUP:
    *DDRx &= ~(1 << PIN);
    *PORTx |= (1 << PIN); // Enable pull-up
    break;
  default:
    break;
  }
}

void GPIOx_Write(volatile uint8_t* PORTx, uint8_t PIN,  bool LOGIC)
{
  if(LOGIC) // Data output: HIGH
  {
    *PORTx |= (1 << PIN);
  }
  else      // Data output: LOW
  {
    *PORTx &= ~(1 << PIN);
  }
}

void GPIOx_WriteToggle(volatile uint8_t* PORTx, uint8_t PIN)
{
  *PORTx ^= (1 << PIN);
}

bool GPIOx_Read(volatile uint8_t* PIN_REG, uint8_t PIN)
{
  /*
  Nếu (*PIN_REG & (1 << PIN)) = 1 => điều kiện đúng => return TRUE. Và ngược lại.
  */
  return (*PIN_REG & (1 << PIN)) != 0;
}

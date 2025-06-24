#include "gpio.h"

void GPIOx_ConfigMode(volatile uint8_t* DDRx, uint8_t PIN, bool MODE)
{
  if(MODE) // Mode: INPUT
  {
    *DDRx &= ~(1 << PIN); // DDRx = 0 -> input
  }
  else   // Mode: OUTPUT
  {
    *DDRx |= (1 << PIN);  // DDRx = 1 -> output
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
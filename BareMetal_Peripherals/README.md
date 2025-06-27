📁 **This folder contains the bare-metal implementation of for the ATmega328P (Arduino Uno microcontroller) without using the Arduino framework.**

## 💡 Use peripheral drivers
### GPIO Driver
`Blink_Led.c`
~~~c
#include <stdio.h>
#include "gpio.h"

int main()
{
  GPIOx_ConfigMode(&DDRB, 5, OUTPUT);

  while(true){
    GPIOx_Write(&PORTB, 5, HIGH);
    delay(1000); // 1s
    GPIOx_Write(&PORTB, 5, LOW);
    delay(1000); // 1s

    //GPIOx_WriteToggle(&PORTB, 5);
    //delay(1000);
  }

  return 0;
}
~~~

### USART Driver
`uart.c`
~~~c
#include <stdio.h>
#include "uart.h"

int main()
{
  USARTx_Init(MYUBRR);

  while(true)
  {
    USARTx_Transmit('1');
    delay(2000);
    USARTx_Print("hello\n");
    delay(2000);
  }

  return 0;
}
~~~

### ADC Driver
`adc.h`
~~~c
#include <stdio.h>
#include "uart.h"
#include "adc.h"

int main()
{
  // Variable declaration
  uint16_t adc_data = 0;

  // Init Driver
  USARTx_Init(MYUBRR);
  ADCx_Init();

  while(true)
  {
    adc_data = ADCx_Read(0); // Pin A0 của arduino uno

    if(adc_data != 0) // Có data
    {
      USARTx_Print(adc_data);
    }
    delay(2000);
  }

  return 0;
}
~~~

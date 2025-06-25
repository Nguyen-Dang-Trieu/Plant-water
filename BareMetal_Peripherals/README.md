📁 **This folder contains the bare-metal implementation of for the ATmega328P (Arduino Uno microcontroller) without using the Arduino framework.**

## 💡 Use peripheral drivers
### GPIO Driver
Blink_Led.c
~~~c
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
uart.c
~~~c
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

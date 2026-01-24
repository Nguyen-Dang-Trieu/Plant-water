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
### I2C Driver
~~~c
#include <stdio.h>
#include "avr/io.h"
#include "util/delay.h"
#include "i2c.h"

int main()
{
  I2Cx_Init(); /// // Khởi tạo I2C

  uint8_t slave_addr = 0x50;
  uint8_t data_to_send = 0x10;
  uint8_t buffer[4];

  while(true)
  {
    I2Cx_Start();

    I2Cx_Write((slave_addr << 1) | 0 ); // Slave address + mode: Write

    I2Cx_Write(data_to_send); // Gửi địa chỉ thanh ghi cần đọc

    I2Cx_Start();

    I2Cx_Write((slave_addr << 1) | 1);  //  mode: Read

    // Đọc dữ liệu từ slave
    for (uint8_t i = 0; i < 4; i++) {
        if (i < 3)
            buffer[i] = I2Cx_Read_ACK();  
        else
            buffer[i] = I2Cx_Read_NACK(); 
    }

    I2Cx_Stop();

    delay(1000); 
  }
  return 0;
}
~~~

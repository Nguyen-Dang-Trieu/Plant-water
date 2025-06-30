#ifndef DHT22_H
#define DHT22_H

#include "Arduino.h"
#include "gpio.h"

struct DHT22 {
  uint8_t data[5];
  uint8_t pin;
  uint32_t lastReadTime, maxCycles;
  bool lastResult;
  uint8_t pullTime; // Time (in usec) to pull up data line before reading 
}

void DHT22_Init(uint8_t usec, uint8_t pin, struct DHT22* sensor);
float DHT22_ReadTemperature(bool select, struct DHT22* sensor);
float DHT22_ConvertCtoF(float celsius, struct DHT22* sensor);
float DHT22_ConvertFtoC(float fahrenheit, struct DHT22* sensor);
float DHT22_ReadHumidity(struct DHT22* sensor);
bool read(struct DHT22* sensor);
uint32_t expectPulse(bool level, struct DHT22* sensor);



#endif // DHT22_T

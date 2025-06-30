#include "DHT22.h"

/*!
 *  @brief  Setup sensor pins and set pull timings 
 *  @param  usec Optionally pass pull-up time (in microseconds) before DHT reading 
 *         
 */
void DHT22_Init(uint8_t usec, uint8_t pin, struct DHT22* sensor)
{
  sensor->pin = pin;
  GPIOx_ConfigMode (&DDRD, &PORTD, pin, INPUT_PULLUP);   // Data-bus's free status is high voltage level
  
  sensor->lastReadTime = millis() - MIN_INTERVAL;

  // Time (in usec) to pull up data line before reading , 
  sensor->pullTime = usec;

  sensor->maxCycles = microsecondsToClockCycles(1000); // 1 millisecond timeout for reading pulses from DHT sensor.
}

/*!
 *  @brief  Read temperature
 *  @param  select
 *          select. Boolean value:
 *          - true = Fahrenheit
 *          - false = Celsius
 *
 *  @return Temperature value in Celsius
 */
float DHT22_ReadTemperature(bool select, struct DHT22* sensor)
{
  float f = NAN;

  if (read(sensor)) { // Check if there is data
    f = ((uint16_t)(sensor->data[2] & 0x7F)) << 8 | sensor->data[3]; // Bit 15 indicates sign, clear it to get the raw temperature
    
    /* Temperature resolution of 16Bit, the previous high; temperature sensor string value is 10 times
    the actual temperature value; - datasheet*/ 
    f *= 0.1; 

    if (sensor->data[2] & 0x80) { 
      //  Negative temperature
      f *= -1;
    }
    
    if (select) {
      f = convertCtoF(f);       
    }
  }

  return f;
}
/*!
 *  @brief  Converts Celsius to Fahrenheit
 *  @param  c
 *          value in Celsius
 *  @return float value in Fahrenheit
 */  
float DHT22_ConvertCtoF(float celsius, struct DHT22* sensor) { return celsius * 1.8 + 32; }
float DHT22_ConvertFtoC(float fahrenheit, struct DHT22* sensor) { return (fahrenheit - 32) * 0.55555; }   

/*!
 *  @brief  Read Humidity
 *
 *  @return float value - humidity in percent
 */
float DHT22_ReadHumidity(struct DHT22* sensor) {
  float f = NAN;
  if (read(sensor)) { // Check if there is data
    f = ((word) sensor->data[0]) << 8 | sensor->data[1];
    f *= 0.1;
  }
  return f;
}

/*!
 *  @brief  Read value from sensor or return last one from less than two
 *seconds.
 *
 *  @return float value
 */
bool read(struct DHT22* sensor)
{
  // Check if sensor was read less than two seconds ago and return early to use last reading.
  uint32_t currenttime = millis();
  if ((currenttime - sensor->lastReadTime) < MIN_INTERVAL) {
    return sensor->lastResult; // return last correct measurement
  }
  sensor->lastReadTime = currenttime;

  // Reset 40 bits of received data to zero.
  sensor->data[0] = sensor->data[1] = sensor->data[2] = sensor->data[3] = sensor->data[4] = 0;

  // Go into high impedence state to let pull-up raise data line level and start the reading process.
  GPIOx_ConfigMode (&DDRD, &PORTD, sensor->pin, INPUT_PULLUP);
  delay(1);

  // First set data line low for a period according to DHT22 sensor 
  GPIOx_ConfigMode (&DDRD, &PORTD, sensor->pin, OUTPUT);
  GPIOx_Write(&PORTD, sensor->pin,  LOW);

  delayMicroseconds(1100); // datasheet says "at least 1ms"


  uint32_t cycles[80];                                    // array: each element is used to hold the LOW or HIGH time period
  {
    // End the start signal by setting data line high for 40 microseconds.
    GPIOx_ConfigMode (&DDRD, &PORTD, sensor->pin, INPUT_PULLUP);

    // Delay a moment to let sensor pull data line low.
    delayMicroseconds(sensor->pullTime);

    // Now start reading the data line to get the value from the DHT22 sensor.

    // First expect a low signal for ~80 microseconds followed by a high signal, for ~80 microseconds again.
    if (expectPulse(LOW) == TIMEOUT) {
      Serial.println(F("DHT timeout waiting for start signal low pulse."));
      sensor->lastResult = false;
      return sensor->lastResult;
    }
    if (expectPulse(HIGH) == TIMEOUT) {
      Serial.println(F("DHT timeout waiting for start signal high pulse."));
      sensor->lastResult = false;
      return sensor->lastResult;
    }

    /*Giải thích thêm:
     * Do mỗi bit để xác định là trạng thái "0" hay "1" thì có 2 khoảng thời gian để xác định
     * + Đối với bit = 0; t = 50us (low state cycle) và tH0 = 22us - 30us (high state cycle)
     * + Đối với bit = 1; t = 50us (low state cycle) và tH1 = 68us - 75us (high state cycle)
     * ==> Ta có 40 bit mà mỗi bit có 2 khoảng thời gian => tổng tất cả là 80 khoảng thời gian nên ta tạo mảng gồm 80 phần tử
     */
    for (int i = 0; i < 80; i += 2) { 
      cycles[i] = expectPulse(LOW);
      cycles[i + 1] = expectPulse(HIGH);
    }
  } // Timing critical code is now complete.

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (int i = 0; i < 40; ++i) { 
    uint32_t lowCycles = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];

    
    if ((lowCycles == TIMEOUT) || (highCycles == TIMEOUT)) {
      Serial.println(F("DHT timeout waiting for pulse."));
      sensor->lastResult = false;
      return sensor->lastResult;
    }

    /* Giải thích tại sao lại là data[i/8]
     *  Vì khi i chạy từ 0 -> 39 chia làm 5 byte (tương ứng với data[0] = 1byte -> data[4] = 1byte khi chia i/8
     */
    sensor->data[i / 8] <<= 1;    
                                  
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      sensor->data[i / 8] |= 1;                                
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }

  /* ---- CHECK DATA ĐÃ NHẬN = BYTE PARITY BIT HAY KHÔNG ----*/ 
  if (sensor->data[4] == ((sensor->data[0] + sensor->data[1] + sensor->data[2] + sensor->data[3]) & 0xFF)) {
    sensor->lastResult = true;
    return sensor->lastResult;
  } else {
    sensor->lastResult = false;
    return sensor->lastResult;
  }
}

uint32_t expectPulse(bool level, struct DHT22* sensor) {
  uint16_t count = 0; 

// On AVR platforms use direct GPIO port access as it's much faster and better
// for catching pulses that are 10's of microseconds in length:
  uint8_t mask = (1 << sensor->pin);
  uint8_t portState = level ? mask : 0;
  while ((*portInputRegister(&PORTD) & mask) == portState) {
    if (count++ >= sensor->maxCycles) {
      return TIMEOUT; // Exceeded timeout, fail.
    }
  }
  return count;
}



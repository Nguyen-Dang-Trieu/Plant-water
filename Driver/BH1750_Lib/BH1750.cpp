#include "BH1750_Driver.h"
#include "Arduino.h"

// Define variables
TwoWire *I2C = &Wire;
Mode BH1750_MODE = UNCONFIGURED;
uint8_t BH1750_MTreg = BH1750_MTreg_DEFAULT;

/**
 * Init BH1750 sensor
 * @param mode is the operating mode
 * @param MTreg is the sensitivity of the sensor
 * @return bool
 *   true if BH1750_ConfigureMode() and BH1750_SetMTreg() success.
 *   false if BH1750_ConfigureMode() is false.
 */
bool BH1750_Init(Mode mode, uint8_t MTreg)
{
  return BH1750_ConfigureMode(mode) && BH1750_SetMTreg(MTreg);
}

/**
 * Configure BH1750 with specified mode
 * @param mode Measurement mode
 * @return bool
 *   true if set mode is success
 *   false if the i2c transmission fails
 */
bool BH1750_ConfigureMode(Mode mode)
{
  uint8_t ack = 5; // Data transmission over time.
  // Check measurement mode is valid
  switch (mode)
  {
  case CONTINUOUS_HIGH_RES_MODE:
  case CONTINUOUS_HIGH_RES_MODE_2:
  case CONTINUOUS_LOW_RES_MODE:
  case ONE_TIME_HIGH_RES_MODE:
  case ONE_TIME_HIGH_RES_MODE_2:
  case ONE_TIME_LOW_RES_MODE:
    // Send mode to sensor
    I2C->beginTransmission(BH1750_ADDRESS);
    I2C->write((uint8_t)mode);
    ack = I2C->endTransmission();

    // Wait a few moments to wake up
    delay(100); // 100 ms
    break;
  default:
    // Invalid measurement mode
    Serial.println("[BH1750] ERROR: Invalid mode ");
    break;
  }
  // ----- End switch -----

  // Check for errors during transmission
  switch (ack)
  {
  case 0:
    BH1750_MODE = mode;
    return true;
  case 1:
    Serial.println(F("[BH1750] ERROR: Data too long to fit in transmit buffer"));
    break;
  case 2:
    Serial.println(F("[BH1750] ERROR: Received NACK on transmit of address"));
    break;
  case 3:
    Serial.println(F("[BH1750] ERROR: Received NACK on transmit of data"));
    break;
  case 4:
    Serial.println(F("[BH1750] ERROR: Other error"));
    break;
  default:
    Serial.println(F("[BH1750] ERROR: undefined error"));
    break;
  }

  return false;
}

/**
 * Set MTreg value to adjust BH1750 sensor sensitivity
 * @param MTreg value of MTreg register
 * @return bool
 *    true if the data transfer is successful
 *    flase if MTreg exceeds allowed value (MIN, MAX) and the I2C transmiision fails
 */
bool BH1750_SetMTreg(uint8_t MTreg)
{
  if (MTreg < BH1750_MTreg_MIN || MTreg > BH1750_MTreg_MAX)
  {
    Serial.println(F("[BH1750] ERROR: MTreg out of range"));
    return false;
  }

  uint8_t ack = 5;

  // When setting MTreg, 2 things are needed: MTreg register value to be set + BH1750 mode

  // ---- Send MTreg to the sensor ----
  I2C->beginTransmission(BH1750_ADDRESS);
  I2C->write(0b01000000 | (MTreg >> 5)); // Take the high 3 bits of byte MTreg
  ack = I2C->endTransmission();

  I2C->beginTransmission(BH1750_ADDRESS);
  I2C->write(0b01100000 | (MTreg & 0b00011111));
  ack = ack | I2C->endTransmission();

  // ---- Send current mode to the sensor ----
  I2C->beginTransmission(BH1750_ADDRESS);
  I2C->write(BH1750_MODE);
  ack = ack | I2C->endTransmission();

  // Wait a few moments
  delay(100); // 100 ms

  // Check the result after transmission
  switch (ack)
  {
  case 0:
    BH1750_MTreg = MTreg; // Store the current value of MTreg
    return true;
  case 1:
    Serial.println(F("[BH1750] ERROR: Data too long to fit in transmit buffer"));
    break;
  case 2:
    Serial.println(F("[BH1750] ERROR: Received NACK on transmit of address"));
    break;
  case 3:
    Serial.println(F("[BH1750] ERROR: Received NACK on transmit of data"));
    break;
  case 4:
    Serial.println(F("[BH1750] ERROR: Other error"));
    break;
  default:
    Serial.println(F("[BH1750] ERROR: undefined error"));
    break;
  }

  return false;
}

/**
 * Read light level from BH1750 sensor
 * The return value range differs if the MTreg value is changed. The global
 * maximum value is noted in the square brackets.
 * @return Light level in lux (0.0 ~ 54612,5 [117758,203])
 * 	   -1 : no valid return value
 */
float BH1750_ReadData()
{
  // Measurement result will be stored here
  float lightLevel = -1.0f;
  float lux_count = 0.0f;    // Illuminance per 1 count
  float MTreg_factor = 1.0f; // 69/X = 1 if X is default value of MTreg

  // ---- Request 2 bytes data from BH1750 sensor ----
  I2C->requestFrom(BH1750_ADDRESS, 2);

  if (I2C->available() == 2)
  {
    uint16_t raw = I2C->read();
    raw = raw << 8;
    raw = raw | I2C->read();
    lightLevel = raw;
  }

  // ---- Raw data processing ----
  if (lightLevel >= 0) // lightLevel is storing raw data.
  {
    if (BH1750_MTreg != BH1750_MTreg_DEFAULT)
    {
      MTreg_factor = (float)BH1750_MTreg_DEFAULT / (float)BH1750_MTreg;

// Print MTreg factor if debug enabled
#ifdef BH1750_DEBUG
      Serial.print(F("[BH1750] MTreg factor: "));
      Serial.println(String(MTreg_factor));
#endif
    }

    if (BH1750_MODE == CONTINUOUS_HIGH_RES_MODE_2 ||
        BH1750_MODE == ONE_TIME_HIGH_RES_MODE_2)
    {
      lux_count = (1.0f / BH1750_CONV_FACTOR) * MTreg_factor * 0.5f;
    }
    else
    {
      lux_count = (1.0f / BH1750_CONV_FACTOR) * MTreg_factor;
    }

// Print lux/count value if debug enabled
#ifdef BH1750_DEBUG
    Serial.print(F("[BH1750] lux/count value: "));
    Serial.println(lux_count);
#endif

    // Convert raw value to lux
    lightLevel = lightLevel * lux_count;
  }

  return lightLevel;
}
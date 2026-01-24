#ifndef BH1750_DRIVER_H
#define BH1750_DRIVER_H

#include <Wire.h>

/* ---- BH1750 ADDRESS I2C ---- */

// 0x23: ADDR ≦ 0.3VCC (Almost use)
// 0x5C: ADDR ≧ 0.7VC
#ifndef BH1750_ADDRESS
#define BH1750_ADDRESS 0x23
#endif


/* ---- BH1750 POWER STATES ---- */
#define BH1750_POWER_DOWN 0x00      // No active state
#define BH1750_POWER_ON   0x01      // Waiting for measurement command
#define BH1750_RESET      0x07      // Reset Data register value. Reset command is not acceptable in Power Down mode.

/* ---- BH1750 Operating Modes ---- */
enum Mode {
  UNCONFIGURED = 0,                 // same as Power Down
  CONTINUOUS_HIGH_RES_MODE = 0x10,  // Measurement at 1 lux resolution. Measurement time is approx 120ms.
  CONTINUOUS_HIGH_RES_MODE_2 = 0x11,// Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
  CONTINUOUS_LOW_RES_MODE = 0x13,   // Measurement at 4 lux resolution. Measurement time is approx 16ms.
  ONE_TIME_HIGH_RES_MODE = 0x20,    // Measurement at 1 lux resolution. Measurement time is approx 120ms.
  ONE_TIME_HIGH_RES_MODE_2 = 0x21,  // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
  ONE_TIME_LOW_RES_MODE = 0x23      // Measurement at 4 lux resolution. Measurement time is approx 16ms.
};

/* ---- Default MTreg value ---- */
#define BH1750_MTreg_DEFAULT 0x45
#define BH1750_MTreg_MIN     0x1F
#define BH1750_MTreg_MAX     0xFE

extern TwoWire* I2C;
extern Mode BH1750_MODE;
extern uint8_t BH1750_MTreg;
const float BH1750_CONV_FACTOR = 1.2f;


bool BH1750_Init(Mode mode, uint8_t MTreg);
bool BH1750_ConfigureMode(Mode mode);
bool BH1750_SetMTreg(uint8_t MTreg);
bool BH1750_MeasurementReady(bool maxWait);   // Not done
float BH1750_ReadData();

#endif
#include <Wire.h>
#include "bus_interface.h"

/* Arduino single-thread → dummy lock */
static int dummy_lock(BusInterface* bus)   { return 0; }
static int dummy_unlock(BusInterface* bus) { return 0; }

static int i2c_transfer(BusInterface* bus,
                        void* device_cfg,
                        uint8_t* tx,
                        uint8_t* rx,
                        uint16_t len)
{
    I2C_DeviceConfig* cfg = (I2C_DeviceConfig*)device_cfg;

    Wire.beginTransmission(cfg->address);
    if (tx) Wire.write(tx, len);    // Ghi
    Wire.endTransmission();

    if (rx) {   // Đọc
        Wire.requestFrom(cfg->address, len);
        for (uint16_t i = 0; i < len && Wire.available(); i++) {
            rx[i] = Wire.read();
        }
    }

    return 0;
}

BusInterface i2c_bus = {
    .type     = BUS_I2C,
    .lock     = dummy_lock,
    .unlock   = dummy_unlock,
    .transfer = i2c_transfer
};

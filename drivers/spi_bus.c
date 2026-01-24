#include <SPI.h>
#include "bus_interface.h"
#include "device_config.h"

static int dummy_lock(BusInterface* bus)   { return 0; } // Chưa sử dụng
static int dummy_unlock(BusInterface* bus) { return 0; } // Chưa sử dụng

static int spi_transfer(BusInterface* bus, void* device_cfg, uint8_t* tx, uint8_t* rx, uint16_t len)
{
    /* Check the input params */
    if(!bus || !device_cfg) return -1;

    SPI_DeviceConfig* cfg = (SPI_DeviceConfig*)device_cfg;

    if(bus->lock) bus->lock(bus); 

    SPI.beginTransaction(
        SPISettings(cfg->clock_hz,
                    cfg->bit_order,
                    cfg->spi_mode)
    );

    digitalWrite(cfg->cs_pin, LOW);

    for (uint16_t i = 0; i < len; i++) {
        uint8_t r = SPI.transfer(tx ? tx[i] : 0xFF);
        if (rx) {
            rx[i] = r;
        }
    }

    digitalWrite(cfg->cs_pin, HIGH);
    SPI.endTransaction();

    if(bus->unlock) bus->unlock(bus);
    return 0;
}

/* SPI bus instance */
BusInterface spi_bus = {
    .type     = BUS_SPI,
    .lock     = NULL,
    .unlock   = NULL,
    .transfer = spi_transfer
};

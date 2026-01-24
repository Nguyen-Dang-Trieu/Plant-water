#include "device_config.h"
#include "bus_interface.h"

static int dummy_lock(BusInterface* bus)   { return 0; }
static int dummy_unlock(BusInterface* bus) { return 0; }

static int uart_transfer(BusInterface* bus, void* device_cfg, uint8_t* tx, uint8_t* rx, uint16_t len)
{
    UART_DeviceConfig* cfg = (UART_DeviceConfig*)device_cfg;

    if(bus->lock) bus->lock(bus);

    // UART WRITE
    if (tx) cfg->serial->write(tx, len); 

    // UART READ
    if (rx) {
        for (uint16_t i = 0; i < len; i++) {
            while (!cfg->serial->available());
            rx[i] = cfg->serial->read();
        }
    }

    if(bus->unlock) bus->unlock(bus);
    return 0;
}

BusInterface uart_bus = {
    .type     = BUS_UART,
    .lock     = NULL,
    .unlock   = NULL,
    .transfer = uart_transfer
};
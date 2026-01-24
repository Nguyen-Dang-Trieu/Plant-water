#ifndef BUS_INTERFACE_H
#define BUS_INTERFACE_H

#include <stdint.h>

typedef enum {
    BUS_SPI,
    BUS_I2C,
    BUS_UART
} BusType;

typedef struct BusInterface BusInterface;

struct BusInterface {
    BusType type;

    int (*lock)(BusInterface* bus);
    int (*unlock)(BusInterface* bus);

    int (*transfer)(BusInterface* bus,
                     void* bus_config,
                     uint8_t* tx,
                     uint8_t* rx,
                     uint16_t len);
};

#endif

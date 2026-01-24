#ifndef DEVICE_H
#define DEVICE_H

#include "bus_interface.h"

typedef struct Device Device;

// App -> Device
struct Device {
    const char* name;   // Tên của thiết bị
    BusInterface* bus;  // Bus mầ thiết bị dùng để giao tiếp
    void* bus_config;   // Cấu hình bus

    int  (*init)(Device* dev);
    int  (*read)(Device* dev, uint8_t* buf, uint16_t len);
    int  (*write)(Device *dev, uint8_t *buf, uint16_t len);
    void (*ioctl)(Device *dev, uint16_t cmd, void *arga); 
};

#endif

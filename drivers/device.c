#include "device.h"

#define MAX_DEVICES 8

static Device* devices[MAX_DEVICES];
static uint8_t device_count = 0;

void device_register(Device* dev)
{
    if (device_count < MAX_DEVICES) {
        devices[device_count++] = dev;
    }
}

void device_init_all(void)
{
    for (uint8_t i = 0; i < device_count; i++) {
        devices[i]->init(devices[i]);
    }
}

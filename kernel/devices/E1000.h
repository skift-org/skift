#pragma once

#include <libsystem/Common.h>

struct __packed E1000RXDescriptor
{
    uint64_t addr;
    uint16_t length;
    uint16_t checksum;
    uint8_t status;
    uint8_t errors;
    uint16_t special;
};

struct __packed E1000TXDescriptor
{
    uint64_t addr;
    uint16_t length;
    uint8_t cso;
    uint8_t cmd;
    uint8_t status;
    uint8_t css;
    uint16_t special;
};

#define E1000_REG_EEPROM 0x0014
#define E1000_REG_MAC_LOW 0x5400
#define E1000_REG_MAC_HIGHT 0x5404

#pragma once

#include "kernel/devices/PCIDevice.h"
#include "kernel/memory/MMIO.h"

#define BGA_ADDRESS 0x01CE
#define BGA_DATA 0x01CF

#define BGA_REG_XRES 0x1
#define BGA_REG_YRES 0x2
#define BGA_REG_BPP 0x3
#define BGA_REG_ENABLE 0x4

#define BGA_DISABLED 0x00
#define BGA_ENABLED 0x01
#define BGA_LINEAR_FRAMEBUFFER 0x40

class BGA : public PCIDevice
{
private:
    int _width;
    int _height;

    RefPtr<MMIORange> _framebuffer;

    void write_register(uint16_t address, uint16_t data);
    uint16_t read_register(uint16_t address);

    Result set_resolution(int width, int height);

public:
    BGA(DeviceAddress address);

    size_t size() override;
    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override;
    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override;
    Result call(IOCall request, void *args) override;
};

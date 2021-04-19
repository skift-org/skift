#pragma once

#include <libutils/Array.h>
#include <skift/Lock.h>

#include "ps2/LegacyDevice.h"

class LegacyATA : public LegacyDevice
{
private:
    Lock _buffer_lock{"legacy-ata"};

    void identify();
    void select();
    void has_failed(uint8_t status);

    void delay(uint16_t io_port);
    void poll(uint16_t io_port);

    // TODO: will have to be uint64_t for LBA48
    void write_lba(uint16_t io_port, uint32_t lba);
    uint8_t read_block(uint8_t *buf, uint32_t lba, size_t size);
    uint8_t write_block(uint8_t *buf, uint32_t lba, size_t size);

    int _bus;
    int _drive;
    Array<uint16_t, 256> _ide_buffer;
    bool _exists = false;
    String _model;
    bool _supports_48lba;
    size_t _num_blocks;

public:
    LegacyATA(DeviceAddress address);

    size_t size() override;

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override;

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override;

    virtual bool did_fail() override { return !_exists; }
};

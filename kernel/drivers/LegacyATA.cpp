#include "kernel/drivers/LegacyATA.h"

// Bus
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

// Drive
#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

// Status flags
#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01

// Commands
#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

// Registers
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

// IO Ports
#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

LegacyATA::LegacyATA(DeviceAddress address) : LegacyDevice(address, DeviceClass::DISK)
{
    switch (address.legacy())
    {
    case LEGACY_ATA0:
        _bus = ATA_PRIMARY;
        _drive = ATA_MASTER;
        break;
    case LEGACY_ATA1:
        _bus = ATA_PRIMARY;
        _drive = ATA_SLAVE;
        break;
    case LEGACY_ATA2:
        _bus = ATA_SECONDARY;
        _drive = ATA_MASTER;
        break;
    case LEGACY_ATA3:
        _bus = ATA_SECONDARY;
        _drive = ATA_SLAVE;
        break;

    default:
        break;
    }

    identify();
}

void LegacyATA::select()
{
    const uint16_t io_port = _bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    out8(io_port + ATA_REG_HDDEVSEL, _drive == ATA_MASTER ? 0xA0 : 0xB0);
}

void LegacyATA::identify()
{
    select();
    const uint16_t io_port = _bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    /* ATA specs say these values must be zero before sending IDENTIFY */
    out8(io_port + ATA_REG_SECCOUNT0, 0);
    out8(io_port + ATA_REG_LBA0, 0);
    out8(io_port + ATA_REG_LBA1, 0);
    out8(io_port + ATA_REG_LBA2, 0);

    /* Now, send IDENTIFY */
    out8(io_port + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    logger_info("Sent IDENTIFY");

    /* Now, read status port */
    uint8_t status = in8(io_port + ATA_REG_STATUS);
    if (status)
    {
        /* Now, poll untill BSY is clear. */
        while ((in8(io_port + ATA_REG_STATUS) & ATA_SR_BSY) != 0)
            ;

        do
        {
            status = in8(io_port + ATA_REG_STATUS);

            if (status & ATA_SR_ERR)
            {
                logger_error("%s%s has ERR set. Disabled.", _bus == ATA_PRIMARY ? "Primary" : "Secondary",
                             _drive == ATA_PRIMARY ? " master" : " slave");

                return;
            }
        } while (!(status & ATA_SR_DRQ));

        logger_info("%s%s is online.", _bus == ATA_PRIMARY ? "Primary" : "Secondary", _drive == ATA_PRIMARY ? " master" : " slave");

        // Read back the data
        for (int i = 0; i < 256; i++)
        {
            _ide_buffer[i] = in16(io_port + ATA_REG_DATA);
        }

        _exists = true;
    }
    else
    {
        logger_error("%s%s does not exist.", _bus == ATA_PRIMARY ? "Primary" : "Secondary",
                     _drive == ATA_PRIMARY ? " master" : " slave");
    }
}

bool LegacyATA::can_read(FsHandle &handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return _exists;
}

ResultOr<size_t> LegacyATA::read(FsHandle &handle, void *buffer, size_t size)
{
    __unused(handle);
    __unused(buffer);
    __unused(size);

    LockHolder holder(_buffer_lock);

    return 0;
}

ResultOr<size_t> LegacyATA::write(FsHandle &handle, const void *buffer, size_t size)
{
    __unused(handle);
    __unused(buffer);
    __unused(size);

    LockHolder holder(_buffer_lock);
    return 0;
}

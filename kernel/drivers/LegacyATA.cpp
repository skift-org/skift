#include "kernel/drivers/LegacyATA.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task.h"

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

// Identify
#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_NUM_BLOCKS0 60
#define ATA_IDENT_NUM_BLOCKS1 61
#define ATA_IDENT_LBA 83
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

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

// LBA modes
#define ATA_28LBA_MAX 0x0FFFFFFF
#define ATA_48LBA_MAX 0xFFFFFFFFFFFF
#define ATA_SECTOR_SIZE 512

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

size_t LegacyATA::size(FsHandle &handle)
{
    __unused(handle);
    return _num_blocks * ATA_SECTOR_SIZE;
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
        for (size_t i = 0; i < _ide_buffer.count(); i++)
        {
            _ide_buffer[i] = in16(io_port + ATA_REG_DATA);
        }

        _exists = true;

        // Parse some infos from the identify buffer
        Array<char, 40> model_buf;
        for (int i = 0; i < 20; i += 1)
        {
            model_buf[i * 2] = _ide_buffer[ATA_IDENT_MODEL + i] >> 8;
            model_buf[i * 2 + 1] = _ide_buffer[ATA_IDENT_MODEL + i] & 0xFF;
        }

        _model = String(model_buf.raw_storage(), model_buf.count());
        _supports_48lba = (_ide_buffer[ATA_IDENT_LBA] >> 10) & 0x1;

        _num_blocks = _ide_buffer[ATA_IDENT_NUM_BLOCKS1] << 16 | _ide_buffer[ATA_IDENT_NUM_BLOCKS0];

        logger_info("IDENITY: Modelname: %s LBA48: %i NB: %i", _model.cstring(), _supports_48lba, _num_blocks);
    }
    else
    {
        logger_error("%s%s does not exist.", _bus == ATA_PRIMARY ? "Primary" : "Secondary",
                     _drive == ATA_PRIMARY ? " master" : " slave");
    }
}

void LegacyATA::delay(uint16_t io_port)
{
    // exactly 400ns
    for (int i = 0; i < 4; i++)
        in8(io_port + ATA_REG_ALTSTATUS);
}

void LegacyATA::poll(uint16_t io_port)
{
    delay(io_port);

    /* Now, poll untill BSY is clear. */
    while ((in8(io_port + ATA_REG_STATUS) & ATA_SR_BSY) != 0)
        task_sleep(scheduler_running(), 10);

    uint8_t status = 0;

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
    return;
}

void LegacyATA::write_lba(uint16_t io_port, uint32_t lba)
{
    const uint8_t cmd = (_drive == ATA_MASTER ? 0xE0 : 0xF0);

    // Write LBA address
    out8(io_port + 1, 0x00);                                                 // Null byte to first port
    out8(io_port + ATA_REG_LBA0, (uint8_t)(lba));                            // First byte of LBA
    out8(io_port + ATA_REG_LBA1, (uint8_t)(lba));                            // Second byte of LBA
    out8(io_port + ATA_REG_LBA2, (uint8_t)(lba));                            // Third byte of LBA
    out8(io_port + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F)))); // High 4-bits of LBA
}

uint8_t LegacyATA::read_block(uint8_t *buf, uint32_t lba, size_t size)
{
    assert(size <= ATA_SECTOR_SIZE);
    const uint16_t io_port = _bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    write_lba(io_port, lba);

    // Issue Read command
    out8(io_port + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    // Poll
    poll(io_port);

    for (unsigned int i = 0; i < (size / 2); i++)
    {
        uint16_t data = in16(io_port + ATA_REG_DATA);
        *(uint16_t *)(buf + i * 2) = data;
    }

    delay(io_port);

    return 1;
}

uint8_t LegacyATA::write_block(uint8_t *buf, uint32_t lba, size_t size)
{
    assert(size <= ATA_SECTOR_SIZE);
    const uint16_t io_port = _bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    write_lba(io_port, lba);

    // Issue Write command
    out8(io_port + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    // Poll
    poll(io_port);

    for (unsigned int i = 0; i < (size / 2); i++)
    {
        uint16_t tmp = (buf[i * 2 + 1] << 8) | buf[i * 2];
        out16(io_port, tmp);
    }

    delay(io_port);

    return 1;
}

ResultOr<size_t> LegacyATA::read(FsHandle &handle, void *buffer, size_t size)
{
    LockHolder holder(_buffer_lock);

    uint32_t start_lba = handle.offset() / ATA_SECTOR_SIZE;
    uint32_t num_blocks = (size / ATA_SECTOR_SIZE) + 1;
    uint8_t *byte_buffer = (uint8_t *)buffer;
    size_t rem_size = size;

    // Read *size* at maximum
    for (unsigned int i = 0; i < num_blocks; i++)
    {
        size_t read_size = MIN(ATA_SECTOR_SIZE, rem_size);
        read_block(byte_buffer, start_lba + i, read_size);
        byte_buffer += ATA_SECTOR_SIZE;
        rem_size -= read_size;
    }

    return size;
}

ResultOr<size_t> LegacyATA::write(FsHandle &handle, const void *buffer, size_t size)
{
    LockHolder holder(_buffer_lock);

    uint32_t start_lba = handle.offset() / ATA_SECTOR_SIZE;
    uint32_t num_blocks = (size / ATA_SECTOR_SIZE) + 1;
    uint8_t *byte_buffer = (uint8_t *)buffer;
    size_t rem_size = size;

    // Write *size* at maximum
    for (unsigned int i = 0; i < num_blocks; i++)
    {
        size_t write_size = MIN(ATA_SECTOR_SIZE, rem_size);
        write_block(byte_buffer, start_lba + i, write_size);
        byte_buffer += ATA_SECTOR_SIZE;
        rem_size -= write_size;
    }

    return size;
}

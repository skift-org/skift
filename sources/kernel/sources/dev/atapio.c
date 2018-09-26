/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/cpu/cpu.h"
#include "kernel/dev/atapio.h"
#include "kernel/logger.h"
#include "sync/atomic.h"

int atapio_common(u8 drive, u32 numblock, u8 count)
{
    outb(0x1F1, 0x00);                            /* NULL byte to port 0x1F1 */
    outb(0x1F2, count);                           /* Sector count */
    outb(0x1F3, (unsigned char)numblock);         /* Low 8 bits of the block address */
    outb(0x1F4, (unsigned char)(numblock >> 8));  /* Next 8 bits of the block address */
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */

    /* Drive indicator, magic bits, and highest 4 bits of the block address */
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));

    return 0;
}

void atapio_wait()
{
    u8 status;

    do
    {
        status = inb(0x1F7);
    } while ((status & 0x80) && !(status & 0x08));
}

int atapio_read(u8 drive, u32 numblock, u8 count, char *buf)
{
    atomic_begin();
    log("ATA::pio read drive:%d block:%d count:%d", drive, numblock, count);
    u16 tmpword;
    int idx;

    atapio_common(drive, numblock, count);
    outb(0x1F7, 0x20);

    /* Wait for the drive to signal that it's ready: */
    atapio_wait();

    for (idx = 0; idx < 256 * count; idx++)
    {
        tmpword = inw(0x1F0);
        buf[idx * 2] = (unsigned char)tmpword;
        buf[idx * 2 + 1] = (unsigned char)(tmpword >> 8);
    }

    log("ATA::pio read done!");
    atomic_end();

    return count;
}

int atapio_write(u8 drive, u32 numblock, u8 count, char *buf)
{
    atomic_begin();
    log("ATA::pio write drive:%d block:%d count:%d", drive, numblock, count);

    u16 tmpword;

    atapio_common(drive, numblock, count);
    outb(0x1F7, 0x30);

    /* Wait for the drive to signal that it's ready: */
    atapio_wait();

    for (int i = 0; i < 256 * count; i++)
    {
        tmpword = (buf[i * 2 + 1] << 8) | buf[i * 2];
        outw(0x1F0, tmpword);
    }

    log("ATA::pio write done!");
    atomic_end();

    return count;
}
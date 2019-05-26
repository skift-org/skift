/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* bga.c: bosh graphic adaptater device driver.                               */

#include <skift/logger.h>

#include <hjert/processor.h>
#include <hjert/dev/bga.h>

void bga_write_register(u16 IndexValue, u16 DataValue)
{
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
}

u16 bga_read_register(u16 IndexValue)
{
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inw(VBE_DISPI_IOPORT_DATA);
}

void bga_set_bank(u16 bank_number)
{
    bga_write_register(VBE_DISPI_INDEX_BANK, bank_number);
}

bool bga_is_available(void)
{
    bool found = false;
    for (u32 i = 0; i < 6; i++)
    {
        if (bga_read_register(VBE_DISPI_INDEX_ID) == 0xB0C0 + i)
        {
            logger_log(LOG_INFO, "BGA video device detected (version %x).", i);
            found = true;
        }
    }
    return found;
}

u32 *bga_get_framebuffer()
{
    // TODO REMOVE: VERY bad hack

    bga_set_bank(0);
    u32 *lfb = 0;
    u32 *text_vid_mem = (u32 *)0xA0000;
    text_vid_mem[0] = 0xA5ADFACE;
    text_vid_mem[1] = 0x12345678;
    text_vid_mem[2] = 0xABCDEF00;

    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
    {
        /* Go find it */
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
        {
            u32* maybe_framebuffer = (u32*)x;

            if (maybe_framebuffer[0] == 0xA5ADFACE &&
                maybe_framebuffer[1] == 0x12345678 &&
                maybe_framebuffer[2] == 0xABCDEF00)
            {
                lfb = (u32 *)x;
            }
        }
    }

    return lfb;
}

/* --- public functions ----------------------------------------------------- */

void bga_mode(u32 width, u32 height)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}
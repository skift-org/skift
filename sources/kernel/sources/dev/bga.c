#include "kernel/cpu/cpu.h"
#include "kernel/dev/bga.h"
#include "kernel/logger.h"

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
            log("BGA video device detected (version %x).", i);
            found = true;
        }
    }
    return found;
}

u32 * bga_get_framebuffer()
{
    bga_set_bank(0);
    u32 * lfb = 0;
    u32 * text_vid_mem = (u32 *)0xA0000;
    text_vid_mem[0] = 0xA5ADFACE;

    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
    {

        /* Go find it */
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
        {
            if (((u32 *)x)[0] == 0xA5ADFACE)
            {
                lfb = (u32 *)x;
            }
        }
    }

    return lfb;
}


/* --- public functions ----------------------------------------------------- */

u32 bga_width = 0;
u32 bga_height = 0;
u32 * bga_framebuffer = NULL;

void bga_setup()
{
    if (bga_is_available())
    {
        bga_mode(1280, 720);
        bga_framebuffer = bga_get_framebuffer();
    }
}

void bga_mode(u32 width, u32 height)
{
    bga_width = width;
    bga_height = height;

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void bga_blit(buffer32_t buffer)
{ 
    for(u32 x = 0; x < bga_width; x++)
    {
        for(u32 y = 0; y < bga_height; x++)
        {
            bga_framebuffer[x + y * bga_width] = buffer[x + y * bga_width];
        }
    }
}

void bga_pixel(u32 x, u32 y, u32 value)
{
    bga_framebuffer[x + y * bga_width] = value;
}
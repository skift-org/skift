#include <string.h>

#include "kernel/dev/bga.h"
#include "kernel/dev/vga_font.h"

#include "kernel/memory.h"
#include "kernel/graphic.h"
#include "kernel/logger.h"
#include "kernel/memory.h"

uint *framebuffer = NULL;

uint graphic_width = 0;
uint graphic_height = 0;

void graphic_setup(uint width, uint height)
{
    graphic_width = width;
    graphic_height = height;

    if (bga_is_available())
    {
        log("Bochs graphic adaptater found!");
        bga_mode(width, height);
        uint* physical_framebuffer = (uint *)bga_get_framebuffer();

        framebuffer = (uint*)memory_alloc_at(memory_kpdir(), (width * height * sizeof(uint)) / PAGE_SIZE, (uint)physical_framebuffer, 0);

        for (uint x = 0; x < graphic_width; x++)
        {
            for (uint y = 0; y < graphic_height; y++)
            {
                graphic_pixel(x, y, 0xAAAAAA);
            }
        }
    }
    else
    {
        log("Warning, no graphic device found!");
        graphic_width = 0;
        graphic_height = 0;
        framebuffer = NULL;
    }

    log("Framebuffer at 0x%x.", framebuffer);
}

void graphic_blit(uint *buffer)
{
    if (framebuffer != NULL)
        memcpy(framebuffer, buffer, graphic_width * graphic_height * sizeof(uint));
}

void graphic_size_char(uint *width, uint *height)
{
    *width = graphic_width / 8;
    *height = graphic_height / 16;
}

void graphic_size_pixel(uint *width, uint *height)
{
    *width = graphic_width;
    *height = graphic_height;
}

int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

void graphic_char(uint x, uint y, uchar c)
{
    if (framebuffer != NULL && x < graphic_width / 8 && y < graphic_height / 16)
    {
        unsigned char *gylph = vgafont16 + (int)c * 16;

        for (int cy = 0; cy < 16; cy++)
        {
            for (int cx = 0; cx < 8; cx++)
            {
                graphic_pixel((x * 8) + cx, (y * 16) + cy, gylph[cy] & mask[cx] ? 0xffffff : 0x0);
            }
        }
    }
}

void graphic_pixel(uint x, uint y, uint color)
{
    if (framebuffer != NULL && x < graphic_width && y < graphic_height)
        framebuffer[x + y * graphic_width] = color;
}

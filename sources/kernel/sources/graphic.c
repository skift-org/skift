#include <string.h>

#include "kernel/dev/bga.h"
#include "kernel/dev/vga_font.h"

#include "kernel/memory.h"
#include "kernel/graphic.h"
#include "kernel/logger.h"
#include "kernel/memory.h"

uint *physical_framebuffer = NULL;
uint *framebuffer = NULL;

uint graphic_width = 0;
uint graphic_height = 0;

/* --- Graphic device setup ------------------------------------------------- */

void graphic_early_setup(uint width, uint height)
{
    if (bga_is_available())
    {
        log("Bochs graphics adaptor found!");
        graphic_width = width;
        graphic_height = height;

        bga_mode(width, height);
        physical_framebuffer = (uint *)bga_get_framebuffer();
        framebuffer = physical_framebuffer;
    }
    else
    {
        log("Warning, no graphic device found!");
        graphic_width = 0;
        graphic_height = 0;
        physical_framebuffer = NULL;
    }

    if (framebuffer != NULL)
    {
        graphic_print(1, 1, "skiftOS booting...");
    }
}

void graphic_setup()
{
    if (physical_framebuffer != NULL)
    {
        uint page_count = PAGE_ALIGN(graphic_width * graphic_height * sizeof(uint)) / PAGE_SIZE;
        framebuffer = (uint *)memory_alloc_at(memory_kpdir(), page_count, (uint)physical_framebuffer, 0);

        graphic_print(1, 1, "skiftOS booting...");
    }

    log("Framebuffer at 0x%x.", framebuffer);
}

/* --- Screen size ---------------------------------------------------------- */

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

/* --- Drawing to the screen ------------------------------------------------ */

void graphic_blit(uint *buffer)
{
    if (framebuffer != NULL)
        memcpy(framebuffer, buffer, graphic_width * graphic_height * sizeof(uint));
}

inline void graphic_pixel(uint x, uint y, uint color)
{
    if (framebuffer != NULL && x < graphic_width && y < graphic_height)
        framebuffer[x + y * graphic_width] = color;
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

void graphic_print(uint x, uint y, char *s)
{
    char c;

    for (size_t i = 0; (c = s[i]); i++)
    {
        graphic_char(x + i, y, c);
    }
}
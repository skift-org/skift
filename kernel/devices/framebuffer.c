#include <skift/atomic.h>
#include <skift/error.h>

#include <hjert/devices/framebuffer.h>

#include "kernel/filesystem.h"
#include "kernel/devices.h"
#include "kernel/paging.h"

/* --- Bochs VBE Extensions driver ------------------------------------------ */

#define VBE_DISPI_BANK_ADDRESS 0xA0000
#define VBE_DISPI_BANK_SIZE_KB 64
#define VBE_DISPI_MAX_XRES 1024
#define VBE_DISPI_MAX_YRES 768
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF
#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_VBE_ENABLED 0x40
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS 0xE0000000

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
            logger_log(LOG_INFO, "BGA framebuffer device detected (version 0x%04x).", 0xB0C0 + i);
            found = true;
        }
    }

    return found;
}

void *bga_get_framebuffer()
{
    atomic_begin();
    paging_disable();

    // FIXME: we should be getting that from PCI

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
            u32 *maybe_framebuffer = (u32 *)x;

            if (maybe_framebuffer[0] == 0xA5ADFACE &&
                maybe_framebuffer[1] == 0x12345678 &&
                maybe_framebuffer[2] == 0xABCDEF00)
            {
                lfb = (u32 *)x;
            }
        }
    }

    paging_enable();
    atomic_end();

    return (void *)lfb;
}

void bga_disable(void)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void bga_mode(u32 width, u32 height)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

/* --- Framebuffer driver --------------------------------------------------- */

static bool is_graphic_mode = false;

static void *framebuffer_addr = NULL;
static int framebuffer_height = -1;
static int framebuffer_width = -1;

int framebuffer_device_ioctl(stream_t *stream, int request, void *args)
{
    UNUSED(stream);

    if (request == FRAMEBUFFER_IOCTL_GET_MODE)
    {
        framebuffer_mode_info_t *mode_info = (framebuffer_mode_info_t *)args;

        mode_info->enable = is_graphic_mode;
        mode_info->width = framebuffer_width;
        mode_info->height = framebuffer_height;
    }
    else if (request == FRAMEBUFFER_IOCTL_SET_MODE)
    {
        framebuffer_mode_info_t *mode_info = (framebuffer_mode_info_t *)args;

        if (mode_info->enable)
        {
            if (mode_info->width > 0 &&
                mode_info->width >= VBE_DISPI_MAX_XRES &&
                mode_info->height > 0 &&
                mode_info->height >= VBE_DISPI_MAX_YRES)
            {
                bga_mode(mode_info->width, mode_info->height);

                if (framebuffer_addr == NULL)
                {
                    framebuffer_addr = bga_get_framebuffer();
                    logger_log(LOG_INFO, "BGA: framebuffer found at 0x%08x", framebuffer_addr);
                }

                framebuffer_width = mode_info->width;
                framebuffer_height = mode_info->height;

                is_graphic_mode = true;

                return -ERR_SUCCESS;
            }
            else
            {
                return -ERR_INVALID_ARGUMENT;
            }
        }
        else
        {
            bga_disable();
            is_graphic_mode = false;

            return -ERR_SUCCESS;
        }
    }
    else if (request == FRAMEBUFFER_IOCTL_BLIT)
    {
        framebuffer_blit_info_t *blit_info = (framebuffer_blit_info_t *)args;
    
        // FIXME: FRAMEBUFFER_IOCTL_BLIT
        UNUSED(blit_info);

        return -ERR_FUNCTION_NOT_IMPLEMENTED;
    }

    return -ERR_INAPPROPRIATE_IOCTL_FOR_DEVICE;
}

void framebuffer_setup(void)
{
    if (bga_is_available())
    {
        device_t framebuffer_device =
            {
                .ioctl = framebuffer_device_ioctl,
            };

        FILESYSTEM_MKDEV(FRAMEBUFFER_DEVICE, framebuffer_device);
    }
    else
    {
        logger_log(LOG_WARNING, "No framebuffer device found!");
    }
}
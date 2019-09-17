/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/atomic.h>
#include <libsystem/error.h>
#include <libsystem/cstring.h>
#include <libmath/math.h>

#include <libdevice/framebuffer.h>

#include "x86/pci.h"
#include "filesystem.h"
#include "devices.h"
#include "paging.h"
#include "memory.h"

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
    out16(VBE_DISPI_IOPORT_INDEX, IndexValue);
    out16(VBE_DISPI_IOPORT_DATA, DataValue);
}

u16 bga_read_register(u16 IndexValue)
{
    out16(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return in16(VBE_DISPI_IOPORT_DATA);
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
            logger_info("BGA framebuffer device detected (version 0x%04x).", 0xB0C0 + i);
            found = true;
        }
    }

    return found;
}

static void framebuffer_pci_get_framebuffer(uint32_t device, uint16_t vendor_id, uint16_t device_id, void *extra)
{
    if (vendor_id == 0x1234 && device_id == 0x1111)
    {
        *((u32 *)extra) = pci_read_field(device, PCI_BAR0, 4);
    }
}

void *bga_get_framebuffer()
{
    u32 lfb_addr = 0;

    pci_scan(framebuffer_pci_get_framebuffer, -1, &lfb_addr);
    logger_info("Framebuffer found at %08x", lfb_addr);

    return (void *)lfb_addr;
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

static void *framebuffer_physical_addr = NULL;
static void *framebuffer_virtual_addr = NULL;
static int framebuffer_height = -1;
static int framebuffer_width = -1;

int framebuffer_device_call(stream_t *stream, int request, void *args)
{
    UNUSED(stream);

    if (request == FRAMEBUFFER_CALL_GET_MODE)
    {
        framebuffer_mode_info_t *mode_info = (framebuffer_mode_info_t *)args;

        mode_info->enable = is_graphic_mode;
        mode_info->width = framebuffer_width;
        mode_info->height = framebuffer_height;

        return -ERR_SUCCESS;
    }
    else if (request == FRAMEBUFFER_CALL_SET_MODE)
    {
        framebuffer_mode_info_t *mode_info = (framebuffer_mode_info_t *)args;

        if (mode_info->enable)
        {
            logger_info("Setting mode to %dx%d...", mode_info->width, mode_info->height);

            if (mode_info->width > 0 &&
                mode_info->width <= VBE_DISPI_MAX_XRES &&
                mode_info->height > 0 &&
                mode_info->height <= VBE_DISPI_MAX_YRES)
            {
                bga_mode(mode_info->width, mode_info->height);

                if (framebuffer_physical_addr == NULL)
                {
                    framebuffer_physical_addr = bga_get_framebuffer();

                    if (framebuffer_physical_addr != NULL)
                    {
                        uint page_count = PAGE_ALIGN(VBE_DISPI_MAX_XRES * VBE_DISPI_MAX_YRES * sizeof(uint)) / PAGE_SIZE;

                        physical_set_used((uint)framebuffer_physical_addr, page_count);

                        framebuffer_virtual_addr = (void *)virtual_alloc(memory_kpdir(), (uint)framebuffer_physical_addr, page_count, 0);
                        if (framebuffer_virtual_addr == NULL)
                        {
                            return -ERR_CANNOT_ALLOCATE_MEMORY;
                        }

                        logger_info("BGA: framebuffer found at 0x%08x", framebuffer_physical_addr);
                    }
                    else
                    {
                        logger_error("BGA: no framebuffer found!");

                        // FIXME: maybe this is note
                        return -ERR_NO_SUCH_DEVICE_OR_ADDRESS;
                    }
                }

                framebuffer_width = mode_info->width;
                framebuffer_height = mode_info->height;

                is_graphic_mode = true;

                return -ERR_SUCCESS;
            }
            else
            {
                logger_warn("Failled to create framebuffer !");
                return -ERR_INVALID_ARGUMENT;
            }
        }
        else
        {
            // FIXME: switch back to textmode
            // bga_disable();
            // is_graphic_mode = false;
            // return -ERR_SUCCESS;

            return -ERR_OPERATION_NOT_SUPPORTED;
        }
    }
    else if (request == FRAMEBUFFER_CALL_BLIT)
    {
        uint *data_to_blit = (uint *)args;

        for (int i = 0; i < framebuffer_width * framebuffer_height; i++)
        {
            ((uint *)framebuffer_virtual_addr)[i] = ((data_to_blit[i] >> 16) & 0x000000ff) |
                                                    (data_to_blit[i] & 0xff00ff00) |
                                                    ((data_to_blit[i] << 16) & 0x00ff0000);
        }

        return -ERR_SUCCESS;
    }
    else if (request == FRAMEBUFFER_CALL_BLITREGION)
    {
        framebuffer_region_t *region = (framebuffer_region_t *)args;

        for (int y = 0; y < region->bound.height; y++)
        {
            for (int x = 0; x < region->bound.width; x++)
            {
                int xx = clamp(x + region->bound.X, 0, framebuffer_width - 1);
                int yy = clamp(y + region->bound.Y, 0, framebuffer_height - 1);

                int i = yy * framebuffer_width + xx;

                uint pixel = ((uint *)region->src)[i];

                ((uint *)framebuffer_virtual_addr)[i] = ((pixel >> 16) & 0x000000ff) |
                                                        (pixel & 0xff00ff00) |
                                                        ((pixel << 16) & 0x00ff0000);
            }
        }

        return -ERR_SUCCESS;
    }
    else
    {
        return -ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void framebuffer_setup(void)
{
    if (bga_is_available())
    {
        device_t framebuffer_device = {
            .call = framebuffer_device_call,
        };

        FILESYSTEM_MKDEV(FRAMEBUFFER_DEVICE, framebuffer_device);
    }
    else
    {
        logger_warn("No framebuffer device found!");
    }
}
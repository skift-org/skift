/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/framebuffer.h>
#include <libmath/math.h>
#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/lock.h>

#include "x86/pci.h"
#include "filesystem.h"
#include "devices.h"
#include "paging.h"
#include "memory.h"
#include "multiboot.h"

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

static void *framebuffer_physical_addr = NULL;
static void *framebuffer_virtual_addr = NULL;
static stream_t *framebuffer_owner = NULL;
static Point framebuffer_size = {-1, -1};

typedef struct
{
    stream_t *owner;
    void *buffer;
} framebuffer_backbuffer_t;

static Lock backbuffer_stack_lock;
static List *backbuffer_stack = NULL;

void *framebuffer_get_buffer(stream_t *owner)
{
    if (framebuffer_owner == owner)
    {
        return framebuffer_virtual_addr;
    }

    list_foreach(framebuffer_backbuffer_t, backbuffer, backbuffer_stack)
    {
        if (backbuffer->owner == owner)
        {
            return backbuffer->buffer;
        }
    }

    return NULL;
}

framebuffer_backbuffer_t *framebuffer_get_backbuffer(stream_t *owner)
{
    list_foreach(framebuffer_backbuffer_t, backbuffer, backbuffer_stack)
    {
        if (backbuffer->owner == owner)
        {
            return backbuffer;
        }
    }

    return NULL;
}

inline void framebuffer_set_pixel(uint *framebuffer, Point size, Point p, uint value)
{
    if ((p.X >= 0 && p.X < size.X) && (p.Y >= 0 && p.Y < size.Y))
        framebuffer[p.X + p.Y * size.X] = value;
}

inline uint framebuffer_get_pixel(uint *framebuffer, Point size, Point p)
{
    int xi = abs((int)p.X % size.X);
    int yi = abs((int)p.Y % size.Y);

    return framebuffer[xi + yi * size.X];
}

void *framebuffer_resize(uint *buffer, Point old_size, Point new_size)
{
    void *resized_framebuffer = malloc((new_size.X * new_size.Y) * sizeof(uint));

    for (int x = 0; x < new_size.X; x++)
    {
        for (int y = 0; y < new_size.Y; y++)
        {
            Point p = (Point){x, y};
            uint pixel_data = framebuffer_get_pixel(buffer, old_size, p);
            framebuffer_set_pixel(resized_framebuffer, new_size, p, pixel_data);
        }
    }

    free(buffer);

    return resized_framebuffer;
}

error_t framebuffer_set_mode_mboot(multiboot_info_t *mboot)
{
    logger_info("Using framebuffer from mboot header.");
    // FIXME: this shoul be ok i guess
    framebuffer_physical_addr = (void *)(uintptr_t)mboot->framebuffer_addr;
    uint page_count = PAGE_ALIGN_UP(mboot->framebuffer_width * mboot->framebuffer_height * (mboot->framebuffer_bpp / 8)) / PAGE_SIZE;
    framebuffer_virtual_addr = (void *)virtual_alloc(memory_kpdir(), (uint)framebuffer_physical_addr, page_count, 0);
    framebuffer_size = (Point){mboot->framebuffer_width, mboot->framebuffer_height};

    return -ERR_SUCCESS;
}

error_t framebuffer_set_mode_bga(Point res)
{
    logger_info("Using framebuffer from BGA device.");

    if (res.X > 0 &&
        res.X <= VBE_DISPI_MAX_XRES &&
        res.Y > 0 &&
        res.Y <= VBE_DISPI_MAX_YRES)
    {
        bga_mode(res.X, res.Y);

        if (framebuffer_physical_addr == NULL)
        {
            framebuffer_physical_addr = bga_get_framebuffer();

            if (framebuffer_physical_addr != NULL)
            {
                uint page_count = PAGE_ALIGN_UP(VBE_DISPI_MAX_XRES * VBE_DISPI_MAX_YRES * sizeof(uint)) / PAGE_SIZE;

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

                // FIXME: maybe this is note the right error code
                return -ERR_NO_SUCH_DEVICE_OR_ADDRESS;
            }
        }

        list_foreach(framebuffer_backbuffer_t, backbuffer, backbuffer_stack)
        {
            backbuffer->buffer = framebuffer_resize(backbuffer->buffer, framebuffer_size, res);
        }

        framebuffer_size = res;

        return -ERR_SUCCESS;
    }
    else
    {
        logger_warn("Failled to create framebuffer !");
        return -ERR_INVALID_ARGUMENT;
    }
}

int framebuffer_device_open(stream_t *stream)
{
    if (stream->flags & IOSTREAM_WRITE)
    {
        lock_acquire(backbuffer_stack_lock);

        if (framebuffer_owner != NULL)
        {
            // Push the old owner to the framebuffer stack.
            framebuffer_backbuffer_t *backbuffer = __malloc(framebuffer_backbuffer_t);

            backbuffer->owner = framebuffer_owner;
            backbuffer->buffer = malloc(framebuffer_size.X * framebuffer_size.Y * sizeof(uint));
            memcpy(backbuffer->buffer, framebuffer_virtual_addr, framebuffer_size.X * framebuffer_size.Y * sizeof(uint));

            list_pushback(backbuffer_stack, backbuffer);
        }

        // Make stream the new owner of the framebuffer.
        framebuffer_owner = stream;
        memset(framebuffer_virtual_addr, 0, framebuffer_size.X * framebuffer_size.Y * sizeof(uint));

        lock_release(backbuffer_stack_lock);
    }

    return 0;
}

int framebuffer_device_close(stream_t *stream)
{
    if (stream->flags & IOSTREAM_WRITE)
    {
        lock_acquire(backbuffer_stack_lock);

        if (framebuffer_owner == stream)
        {
            // If stream is the owner, pop from the backbuffer stack.
            framebuffer_backbuffer_t *backbuffer = NULL;
            if (list_popback(backbuffer_stack, (void **)&backbuffer))
            {
                // Switch to the new backbuffer
                framebuffer_owner = backbuffer->owner;
                memcpy(framebuffer_virtual_addr, backbuffer->buffer, framebuffer_size.X * framebuffer_size.Y * sizeof(uint));

                free(backbuffer->buffer);
                free(backbuffer);
            }
        }
        else
        {
            // Else remove stream from the backbuffer stack.
            framebuffer_backbuffer_t *backbuffer = framebuffer_get_backbuffer(stream);
            list_remove(backbuffer_stack, backbuffer);
            free(backbuffer->buffer);
            free(backbuffer);
        }

        lock_release(backbuffer_stack_lock);
    }

    return 0;
}

int framebuffer_device_call(stream_t *stream, int request, void *args)
{
    __unused(stream);

    lock_acquire(backbuffer_stack_lock);

    if (request == FRAMEBUFFER_CALL_GET_MODE)
    {
        framebuffer_mode_arg_t *mode_info = (framebuffer_mode_arg_t *)args;

        mode_info->size = framebuffer_size;

        lock_release(backbuffer_stack_lock);

        return -ERR_SUCCESS;
    }
    else if (request == FRAMEBUFFER_CALL_SET_MODE)
    {
        framebuffer_mode_arg_t *mode_info = (framebuffer_mode_arg_t *)args;

        logger_info("Setting mode to %dx%d...", mode_info->size.X, mode_info->size.Y);

        int result = framebuffer_set_mode_bga(mode_info->size);

        lock_release(backbuffer_stack_lock);

        return result;
    }
    else if (request == FRAMEBUFFER_CALL_BLIT)
    {
        if (stream->flags & IOSTREAM_WRITE)
        {
            framebuffer_blit_args_t *blitargs = args;

            for (int y = 0; y < blitargs->size.Y; y++)
            {
                for (int x = 0; x < blitargs->size.X; x++)
                {
                    uint source_pixel_data = framebuffer_get_pixel(blitargs->buffer, blitargs->size, (Point){x, y});

                    uint converted_pixel_data = ((source_pixel_data >> 16) & 0x000000ff) |
                                                ((source_pixel_data)&0xff00ff00) |
                                                ((source_pixel_data << 16) & 0x00ff0000);

                    framebuffer_set_pixel(framebuffer_get_buffer(stream), framebuffer_size, (Point){x, y}, converted_pixel_data);
                }
            }
            lock_release(backbuffer_stack_lock);

            return -ERR_SUCCESS;
        }
        else
        {
            lock_release(backbuffer_stack_lock);

            return -ERR_READ_ONLY_STREAM;
        }
    }
    else if (request == FRAMEBUFFER_CALL_BLITREGION)
    {
        if (stream->flags & IOSTREAM_WRITE)
        {
            framebuffer_blitregion_args_t *region = (framebuffer_blitregion_args_t *)args;

            for (int y = 0; y < region->region_to_blit.height; y++)
            {
                for (int x = 0; x < region->region_to_blit.width; x++)
                {
                    int xx = x + region->region_to_blit.X;
                    int yy = y + region->region_to_blit.Y;

                    uint source_pixel_data = framebuffer_get_pixel(region->buffer, region->size, (Point){xx, yy});

                    uint converted_pixel_data = ((source_pixel_data >> 16) & 0x000000ff) |
                                                ((source_pixel_data)&0xff00ff00) |
                                                ((source_pixel_data << 16) & 0x00ff0000);

                    framebuffer_set_pixel(framebuffer_get_buffer(stream), framebuffer_size, (Point){xx, yy}, converted_pixel_data);
                }
            }

            lock_release(backbuffer_stack_lock);

            return -ERR_SUCCESS;
        }
        else
        {
            lock_release(backbuffer_stack_lock);

            return -ERR_READ_ONLY_STREAM;
        }
    }
    else
    {
        lock_release(backbuffer_stack_lock);

        return -ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void framebuffer_setup(multiboot_info_t *mboot)
{
    if (mboot->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB || bga_is_available())
    {
        lock_init(backbuffer_stack_lock);
        backbuffer_stack = list_create();

        if (mboot->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
        {
            framebuffer_set_mode_mboot(mboot);
        }
        else
        {
            framebuffer_set_mode_bga((Point){800, 600});
        }

        device_t framebuffer_device = {
            .open = framebuffer_device_open,
            .close = framebuffer_device_close,
            .call = framebuffer_device_call,
        };

        FILESYSTEM_MKDEV(FRAMEBUFFER_DEVICE, framebuffer_device);
    }
    else
    {
        logger_warn("No framebuffer device found!");
    }
}
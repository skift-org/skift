/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/math.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/lock.h>

#include <thirdparty/multiboot/Multiboot.h>

#include "kernel/device/Device.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/x86/pci.h"

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
static FsHandle *framebuffer_owner = NULL;
static int framebuffer_width;
static int framebuffer_height;

typedef struct
{
    FsHandle *owner;
    void *buffer;
} framebuffer_backbuffer_t;

static Lock backbuffer_stack_lock;
static List *backbuffer_stack = NULL;

void *framebuffer_get_buffer(FsHandle *owner)
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

framebuffer_backbuffer_t *framebuffer_get_backbuffer(FsHandle *owner)
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

inline void framebuffer_set_pixel(uint *framebuffer, int width, int height, int x, int y, uint32_t value)
{
    if ((x >= 0 && x < width) && (y >= 0 && y < height))
        framebuffer[x + y * width] = value;
}

inline uint32_t framebuffer_get_pixel(uint *framebuffer, int width, int height, int x, int y)
{
    int xi = abs((int)x % width);
    int yi = abs((int)y % height);

    return framebuffer[xi + yi * width];
}

void *framebuffer_resize(uint *buffer, int old_width, int old_height, int new_width, int new_height)
{
    void *resized_framebuffer = malloc((new_width * new_height) * sizeof(uint));

    for (int x = 0; x < new_width; x++)
    {
        for (int y = 0; y < new_height; y++)
        {
            uint pixel_data = framebuffer_get_pixel(buffer, old_width, old_height, x, y);
            framebuffer_set_pixel(resized_framebuffer, new_width, new_height, x, y, pixel_data);
        }
    }

    free(buffer);

    return resized_framebuffer;
}

Result framebuffer_set_mode_mboot(multiboot_info_t *mboot)
{
    logger_info("Using framebuffer from mboot header.");
    // FIXME: this shoul be ok i guess
    framebuffer_physical_addr = (void *)(uintptr_t)mboot->framebuffer_addr;
    uint page_count = PAGE_ALIGN_UP(mboot->framebuffer_width * mboot->framebuffer_height * (mboot->framebuffer_bpp / 8)) / PAGE_SIZE;
    framebuffer_virtual_addr = (void *)virtual_alloc(memory_kpdir(), (uint)framebuffer_physical_addr, page_count, 0);
    framebuffer_width = mboot->framebuffer_width;
    framebuffer_height = mboot->framebuffer_height;

    return SUCCESS;
}

Result framebuffer_set_mode_bga(int width, int height)
{
    logger_info("Using framebuffer from BGA device.");

    if (width > 0 &&
        width <= VBE_DISPI_MAX_XRES &&
        height > 0 &&
        height <= VBE_DISPI_MAX_YRES)
    {
        bga_mode(width, height);

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
                    return ERR_CANNOT_ALLOCATE_MEMORY;
                }

                logger_info("BGA: framebuffer found at 0x%08x", framebuffer_physical_addr);
            }
            else
            {
                logger_error("BGA: no framebuffer found!");

                // FIXME: maybe this is note the right error code
                return ERR_NO_SUCH_DEVICE_OR_ADDRESS;
            }
        }

        list_foreach(framebuffer_backbuffer_t, backbuffer, backbuffer_stack)
        {
            backbuffer->buffer = framebuffer_resize(backbuffer->buffer, framebuffer_width, framebuffer_height, width, height);
        }

        framebuffer_width = width;
        framebuffer_height = height;

        return SUCCESS;
    }
    else
    {
        logger_warn("Failled to create framebuffer !");
        return ERR_INVALID_ARGUMENT;
    }
}

int framebuffer_FsOperationOpen(FsNode *node, FsHandle *handle)
{
    __unused(node);

    if (handle->flags & OPEN_WRITE)
    {
        lock_acquire(backbuffer_stack_lock);

        if (framebuffer_owner != NULL)
        {
            // Push the old owner to the framebuffer stack.
            framebuffer_backbuffer_t *backbuffer = __create(framebuffer_backbuffer_t);

            backbuffer->owner = framebuffer_owner;
            backbuffer->buffer = malloc(framebuffer_width * framebuffer_height * sizeof(uint));
            memcpy(backbuffer->buffer, framebuffer_virtual_addr, framebuffer_width * framebuffer_height * sizeof(uint));

            list_pushback(backbuffer_stack, backbuffer);
        }

        // Make stream the new owner of the framebuffer.
        framebuffer_owner = handle;
        memset(framebuffer_virtual_addr, 0, framebuffer_width * framebuffer_height * sizeof(uint));

        lock_release(backbuffer_stack_lock);
    }

    return SUCCESS;
}

void framebuffer_FsOperationClose(FsNode *node, FsHandle *handle)
{
    __unused(node);

    if (handle->flags & OPEN_WRITE)
    {
        lock_acquire(backbuffer_stack_lock);

        if (framebuffer_owner == handle)
        {
            // If handle is the owner, pop from the backbuffer stack.
            framebuffer_backbuffer_t *backbuffer = NULL;
            if (list_popback(backbuffer_stack, (void **)&backbuffer))
            {
                // Switch to the new backbuffer
                framebuffer_owner = backbuffer->owner;
                memcpy(framebuffer_virtual_addr, backbuffer->buffer, framebuffer_width * framebuffer_height * sizeof(uint));

                free(backbuffer->buffer);
                free(backbuffer);
            }
        }
        else
        {
            // Else remove stream from the backbuffer stack.
            framebuffer_backbuffer_t *backbuffer = framebuffer_get_backbuffer(handle);
            list_remove(backbuffer_stack, backbuffer);
            free(backbuffer->buffer);
            free(backbuffer);
        }

        lock_release(backbuffer_stack_lock);
    }
}

Result framebuffer_FsOperationCall(FsNode *node, FsHandle *handle, int request, void *args)
{
    __unused(node);

    lock_acquire(backbuffer_stack_lock);

    Result result = SUCCESS;

    if (request == IOCALL_DISPLAY_GET_MODE)
    {
        IOCallDisplayModeArgs *mode_info = (IOCallDisplayModeArgs *)args;

        mode_info->width = framebuffer_width;
        mode_info->height = framebuffer_height;
    }
    else if (request == IOCALL_DISPLAY_SET_MODE)
    {
        IOCallDisplayModeArgs *mode_info = (IOCallDisplayModeArgs *)args;

        result = framebuffer_set_mode_bga(mode_info->width, mode_info->height);
    }
    else if (request == IOCALL_DISPLAY_BLIT)
    {
        if (!handle_has_flags(handle, OPEN_WRITE))
        {
            result = ERR_READ_ONLY_STREAM;
            goto cleanup_and_return;
        }

        IOCallDisplayBlitArgs *region = (IOCallDisplayBlitArgs *)args;

        for (int y = 0; y < region->blit_height; y++)
        {
            for (int x = 0; x < region->blit_width; x++)
            {
                int xx = x + region->blit_x;
                int yy = y + region->blit_y;

                uint32_t source_pixel_data = framebuffer_get_pixel(
                    region->buffer,
                    region->buffer_width,
                    region->buffer_height,
                    xx, yy);

                uint32_t converted_pixel_data = ((source_pixel_data >> 16) & 0x000000ff) |
                                                ((source_pixel_data)&0xff00ff00) |
                                                ((source_pixel_data << 16) & 0x00ff0000);

                framebuffer_set_pixel(
                    framebuffer_get_buffer(handle),
                    framebuffer_width,
                    framebuffer_height,
                    xx, yy,
                    converted_pixel_data);
            }
        }
    }
    else
    {
        result = ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }

cleanup_and_return:
    lock_release(backbuffer_stack_lock);

    return result;
}

bool framebuffer_initialize(multiboot_info_t *mboot)
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
            framebuffer_set_mode_bga(800, 600);
        }

        FsNode *framebuffer_device = __create(FsNode);
        fsnode_init(framebuffer_device, FSNODE_DEVICE);

        FSNODE(framebuffer_device)->open = (FsOperationOpen)framebuffer_FsOperationOpen;
        FSNODE(framebuffer_device)->close = (FsOperationClose)framebuffer_FsOperationClose;
        FSNODE(framebuffer_device)->call = (FsOperationCall)framebuffer_FsOperationCall;

        Path *framebuffer_device_path = path_create("/dev/framebuffer");
        filesystem_link(framebuffer_device_path, framebuffer_device);
        path_destroy(framebuffer_device_path);

        return true;
    }
    else
    {
        logger_warn("No framebuffer device found!");

        return false;
    }
}
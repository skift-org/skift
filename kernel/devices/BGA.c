#include <libmath/MinMax.h>
#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Virtual.h"

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

void bga_set_mode(u32 width, u32 height)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

static uintptr_t framebuffer_physical = 0;
static uintptr_t framebuffer_virtual = 0;

Result bga_iocall(FsNode *node, FsHandle *handle, IOCall iocall, void *args)
{
    __unused(node);
    __unused(handle);

    if (iocall == IOCALL_DISPLAY_GET_MODE)
    {
        IOCallDisplayModeArgs *mode = (IOCallDisplayModeArgs *)args;

        mode->width = VBE_DISPI_MAX_XRES;
        mode->height = VBE_DISPI_MAX_YRES;

        return SUCCESS;
    }
    else if (iocall == IOCALL_DISPLAY_BLIT)
    {
        IOCallDisplayBlitArgs *blit = (IOCallDisplayBlitArgs *)args;

        for (int y = MAX(0, blit->blit_y); y < MIN(VBE_DISPI_MAX_YRES, blit->blit_y + blit->blit_height); y++)
        {
            for (int x = MAX(0, blit->blit_x); x < MIN(VBE_DISPI_MAX_XRES, blit->blit_x + blit->blit_width); x++)
            {
                uint32_t pixel = blit->buffer[y * blit->buffer_width + x];

                uint32_t converted_pixel = ((pixel >> 16) & 0x000000ff) |
                                           ((pixel)&0xff00ff00) |
                                           ((pixel << 16) & 0x00ff0000);

                ((uint32_t *)framebuffer_virtual)[y * VBE_DISPI_MAX_XRES + x] = converted_pixel;
            }
        }

        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

bool bga_match(DeviceInfo info)
{
    return info.vendor_id == 0x1234 && info.device_id == 0x1111;
}

void bga_initialize(DeviceInfo info)
{
    bga_set_mode(VBE_DISPI_MAX_XRES, VBE_DISPI_MAX_YRES);
    framebuffer_physical = pci_read_field(info.device, PCI_BAR0, 4);
    framebuffer_virtual = virtual_alloc(
        &kpdir,
        framebuffer_physical,
        PAGE_ALIGN_UP(VBE_DISPI_MAX_XRES * VBE_DISPI_MAX_XRES * sizeof(uint32_t)) / PAGE_SIZE,
        false);

    if (framebuffer_virtual == 0)
    {
        logger_error("Failed to allocate memory for the framebuffer!");
        return;
    }

    FsNode *file = __create(FsNode);
    file->call = (FsOperationCall)bga_iocall;
    fsnode_init(file, FILE_TYPE_DEVICE);

    Path *path = path_create("/dev/framebuffer");
    filesystem_link(path, file);
    path_destroy(path);
}
#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/thread/Atomic.h>

#include "arch/x86/x86.h"

#include "kernel/bus/PCI.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/graphics/Graphics.h"
#include "kernel/memory/Virtual.h"

#define VBE_DISPI_BANK_ADDRESS 0xA0000
#define VBE_DISPI_BANK_SIZE_KB 64

#define VBE_DISPI_DEFAULT_XRES 1280
#define VBE_DISPI_DEFAULT_YRES 768
#define VBE_DISPI_MAX_XRES 1920
#define VBE_DISPI_MAX_YRES 1080

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

static uintptr_t framebuffer_physical = 0;
static uintptr_t framebuffer_virtual = 0;
static int framebuffer_width = 0;
static int framebuffer_height = 0;

void bga_write_register(uint16_t IndexValue, uint16_t DataValue)
{
    out16(VBE_DISPI_IOPORT_INDEX, IndexValue);
    out16(VBE_DISPI_IOPORT_DATA, DataValue);
}

uint16_t bga_read_register(uint16_t IndexValue)
{
    out16(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return in16(VBE_DISPI_IOPORT_DATA);
}

void bga_set_mode(int width, int height)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    framebuffer_width = width;
    framebuffer_height = height;
}

Result bga_iocall(FsNode *node, FsHandle *handle, IOCall iocall, void *args)
{
    __unused(node);
    __unused(handle);

    if (iocall == IOCALL_DISPLAY_GET_MODE)
    {
        IOCallDisplayModeArgs *mode = (IOCallDisplayModeArgs *)args;

        mode->width = framebuffer_width;
        mode->height = framebuffer_height;

        return SUCCESS;
    }
    else if (iocall == IOCALL_DISPLAY_BLIT)
    {
        IOCallDisplayBlitArgs *blit = (IOCallDisplayBlitArgs *)args;

        for (int y = MAX(0, blit->blit_y); y < MIN(framebuffer_height, blit->blit_y + blit->blit_height); y++)
        {
            for (int x = MAX(0, blit->blit_x); x < MIN(framebuffer_width, blit->blit_x + blit->blit_width); x++)
            {
                uint32_t pixel = blit->buffer[y * blit->buffer_width + x];

                uint32_t converted_pixel = ((pixel >> 16) & 0x000000ff) |
                                           ((pixel)&0xff00ff00) |
                                           ((pixel << 16) & 0x00ff0000);

                ((uint32_t *)framebuffer_virtual)[y * framebuffer_width + x] = converted_pixel;
            }
        }

        return SUCCESS;
    }
    else if (iocall == IOCALL_DISPLAY_SET_MODE)
    {
        IOCallDisplayModeArgs *mode = (IOCallDisplayModeArgs *)args;
        bga_set_mode(mode->width, mode->height);
        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

class BGA : public FsNode
{
private:
public:
    BGA() : FsNode(FILE_TYPE_DEVICE)
    {
        call = (FsNodeCallCallback)bga_iocall;
    }

    ~BGA() {}
};

bool bga_match(DeviceInfo info)
{
    return /* QEMU */ (info.pci_device.vendor == 0x1234 && info.pci_device.device == 0x1111) ||
           /* VBOX */ (info.pci_device.vendor == 0x80ee && info.pci_device.device == 0xbeef);
}

void bga_initialize(DeviceInfo info)
{
    AtomicHolder holder;

    bga_set_mode(VBE_DISPI_DEFAULT_XRES, VBE_DISPI_DEFAULT_YRES);
    framebuffer_physical = pci_device_read_bar(info.pci_device, 0) & 0xFFFFFFF0;
    framebuffer_virtual = virtual_alloc(
                              &kpdir,
                              (MemoryRange){
                                  framebuffer_physical,
                                  PAGE_ALIGN_UP(VBE_DISPI_MAX_XRES * VBE_DISPI_MAX_XRES * sizeof(uint32_t)),
                              },
                              MEMORY_NONE)
                              .base();

    if (framebuffer_virtual == 0)
    {
        logger_error("Failed to allocate memory for the framebuffer!");
        return;
    }

    graphic_did_find_framebuffer();

    filesystem_link_and_take_ref_cstring(FRAMEBUFFER_DEVICE_PATH, new BGA());
}

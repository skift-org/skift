#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/thread/Atomic.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/graphics/Graphics.h"
#include "kernel/memory/Virtual.h"

static uintptr_t _framebuffer_physical = 0;
static uintptr_t _framebuffer_virtual = 0;
static int _framebuffer_width = 0;
static int _framebuffer_height = 0;
static int _framebuffer_pitch = 0;

class Framebuffer : public FsNode
{
private:
    /* data */
public:
    Framebuffer(/* args */) : FsNode(FILE_TYPE_DEVICE)
    {
    }

    ~Framebuffer()
    {
    }

    Result call(FsHandle &handle, IOCall iocall, void *args) override
    {
        __unused(handle);

        if (iocall == IOCALL_DISPLAY_GET_MODE)
        {
            IOCallDisplayModeArgs *mode = (IOCallDisplayModeArgs *)args;

            mode->width = _framebuffer_width;
            mode->height = _framebuffer_height;

            return SUCCESS;
        }
        else if (iocall == IOCALL_DISPLAY_BLIT)
        {
            IOCallDisplayBlitArgs *blit = (IOCallDisplayBlitArgs *)args;

            AtomicHolder holder;

            for (int y = MAX(0, blit->blit_y); y < MIN(_framebuffer_height, blit->blit_y + blit->blit_height); y++)
            {
                for (int x = MAX(0, blit->blit_x); x < MIN(_framebuffer_width, blit->blit_x + blit->blit_width); x++)
                {
                    uint32_t pixel = blit->buffer[y * blit->buffer_width + x];

                    uint32_t converted_pixel = ((pixel >> 16) & 0x000000ff) |
                                               ((pixel)&0xff00ff00) |
                                               ((pixel << 16) & 0x00ff0000);

                    ((uint32_t *)_framebuffer_virtual)[y * (_framebuffer_pitch / 4) + x] = converted_pixel;
                }
            }

            return SUCCESS;
        }
        else
        {
            return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
        }
    }
};

void framebuffer_initialize(Multiboot *multiboot)
{
    AtomicHolder holder;

    _framebuffer_width = multiboot->framebuffer_width;
    _framebuffer_height = multiboot->framebuffer_height;
    _framebuffer_pitch = multiboot->framebuffer_pitch;

    _framebuffer_physical = multiboot->framebuffer_addr;
    _framebuffer_virtual = virtual_alloc(
                               &kpdir,
                               (MemoryRange){
                                   _framebuffer_physical,
                                   PAGE_ALIGN_UP(_framebuffer_width * _framebuffer_height * sizeof(uint32_t)),
                               },
                               MEMORY_NONE)
                               .base();

    if (_framebuffer_virtual == 0)
    {
        logger_error("Failed to allocate memory for the framebuffer!");
        return;
    }

    graphic_did_find_framebuffer();

    filesystem_link_and_take_ref_cstring(FRAMEBUFFER_DEVICE_PATH, new Framebuffer());
}

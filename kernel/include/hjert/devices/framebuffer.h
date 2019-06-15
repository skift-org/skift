#pragma once

#include <skift/runtime.h>

#define FRAMEBUFFER_DEVICE "/dev/fb"
#define FRAMEBUFFER_IOCTL_SET_MODE 0
#define FRAMEBUFFER_IOCTL_GET_MODE 1
#define FRAMEBUFFER_IOCTL_BLIT 2
#define FRAMEBUFFER_IOCTL_BLITREGION 3

#define FRAMEBUFFER_CHANNEL     "#dev:framebuffer"
#define FRAMEBUFFER_MODE_CHANGE "framebuffer.modechange"

typedef struct
{
    bool enable;

    int width;
    int height;
} framebuffer_mode_info_t;

typedef struct
{
    void* src;
    
    int src_height;
    int src_width;

    int dest_x;
    int dest_y;
} framebuffer_blit_info_t;

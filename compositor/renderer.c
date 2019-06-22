/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/assert.h>
#include <skift/error.h>
#include <skift/logger.h>

#include <hjert/devices/framebuffer.h>

#include "compositor/renderer.h"

hideo_renderer_t *hideo_renderer(point_t prefered_resolution)
{
    hideo_renderer_t *this = OBJECT(hideo_renderer);

    this->device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (this->device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        free(this);
        return false;
    }

    framebuffer_mode_info_t mode_info = {true, prefered_resolution.X, prefered_resolution.Y};

    if (iostream_ioctl(this->device, FRAMEBUFFER_IOCTL_SET_MODE, &mode_info) < 0)
    {
        error_print("Failled to set device " FRAMEBUFFER_DEVICE " mode");
        iostream_close(this->device);
        free(this);
        return false;
    }

    this->backbuffer = bitmap(mode_info.width, mode_info.height);
    this->painter = painter(this->backbuffer);

    return this;
}

void hideo_renderer_delete(hideo_renderer_t *this)
{
    iostream_close(this->device);
    painter_delete(this->painter);
    bitmap_delete(this->backbuffer);
}

rectangle_t hideo_renderer_screen_bound(hideo_renderer_t* this)
{
    return bitmap_bound(this->backbuffer);
}

void hideo_renderer_dirty(hideo_renderer_t *this, rectangle_t rect)
{
    assert(this->dirty_index < HIDEO_RENDERER_MAX_DIRTY);

    for (int i = 0; i < this->dirty_index; i++)
    {
        if (rectange_colide(this->dirty[i], rect))
        {
            this->dirty[i] = rectangle_merge(this->dirty[i], rect);
            return;
        }
    }
    
    this->dirty[this->dirty_index] = rect;

    this->dirty_index++;
}

void hideo_renderer_blit(hideo_renderer_t *this)
{
    // FIXME: we should maybe merge rects
    logger_log(LOG_DEBUG, "Blitting %d regions", this->dirty_index);

    for (int i = 0; i < this->dirty_index; i++)
    {
        framebuffer_region_t damaged_region = (framebuffer_region_t){.src = this->backbuffer->buffer, .bound = this->dirty[i]};
        iostream_ioctl(this->device, FRAMEBUFFER_IOCTL_BLITREGION, &damaged_region);
    }

    this->dirty_index = 0;
}

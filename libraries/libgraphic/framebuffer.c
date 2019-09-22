#include <libsystem/error.h>
#include <libsystem/logger.h>
#include <libdevice/framebuffer.h>
#include <libgraphic/framebuffer.h>

framebuffer_t *framebuffer_open(void)
{
    iostream_t *framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        return NULL;
    }

    framebuffer_mode_arg_t mode_info;

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_GET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return NULL;
    }

    logger_info("Framebuffer create with graphic mode %dx%d.", mode_info.size.X, mode_info.size.Y);

    bitmap_t *framebuffer_backbuffer = bitmap(mode_info.size.X, mode_info.size.Y);

    painter_t *framebuffer_painter = painter(framebuffer_backbuffer);

    framebuffer_t *this = MALLOC(framebuffer_t);

    this->device = framebuffer_device;
    this->backbuffer = framebuffer_backbuffer;
    this->painter = framebuffer_painter;
    this->is_dirty = false;

    this->width = mode_info.size.X;
    this->height = mode_info.size.Y;

    return this;
}

bool framebuffer_set_mode(framebuffer_t *this, int resx, int resy)
{
    framebuffer_mode_arg_t mode_info = (framebuffer_mode_arg_t){
        .size = (point_t){resx, resy},
    };

    if (iostream_call(this->device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) != 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return false;
    }

    object_release(this->backbuffer);
    object_release(this->painter);

    this->backbuffer = bitmap(resx, resy);
    this->painter = painter(this->backbuffer);

    this->width = mode_info.size.X;
    this->height = mode_info.size.Y;

    framebuffer_mark_dirty(this, bitmap_bound(this->backbuffer));

    return true;
}

rectangle_t framebuffer_bound(framebuffer_t *this)
{
    return (rectangle_t){{0, 0, this->width, this->height}};
}

void framebuffer_mark_dirty(framebuffer_t *this, rectangle_t bound)
{
    if (rectange_colide(bitmap_bound(this->backbuffer), bound))
    {
        bound = rectangle_child(bitmap_bound(this->backbuffer), bound);

        if (this->is_dirty)
        {
            this->dirty_bound = rectangle_merge(this->dirty_bound, bound);
        }
        else
        {
            this->is_dirty = true;
            this->dirty_bound = rectangle_child(bitmap_bound(this->backbuffer), bound);
        }
    }
}

void framebuffer_mark_dirty_all(framebuffer_t *this)
{
    this->is_dirty = true;
    this->dirty_bound = bitmap_bound(this->backbuffer);
}

void framebuffer_blit_dirty(framebuffer_t *this)
{
    if (this->is_dirty)
    {
        framebuffer_blitregion_args_t args;

        args.buffer = this->backbuffer->buffer;
        args.size = bitmap_bound(this->backbuffer).size;
        args.region_to_blit = this->dirty_bound;

        if (iostream_call(this->device, FRAMEBUFFER_CALL_BLITREGION, &args) != 0)
        {
            error_print("Failled to blit to the framebuffer");
        }

        this->is_dirty = false;
    }
}

void framebuffer_blit(framebuffer_t *this)
{
    framebuffer_blit_args_t args;

    args.buffer = this->backbuffer->buffer;
    args.size = bitmap_bound(this->backbuffer).size;

    if (iostream_call(this->device, FRAMEBUFFER_CALL_BLIT, &args) != 0)
    {
        error_print("Failled to blit to the framebuffer");
    }

    this->is_dirty = false;
}

void framebuffer_close(framebuffer_t *this)
{
    iostream_close(this->device);
    object_release(this->backbuffer);
    object_release(this->painter);

    free(this);
}
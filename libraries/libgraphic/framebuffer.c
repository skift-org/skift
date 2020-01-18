#include <libdevice/framebuffer.h>
#include <libgraphic/framebuffer.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>

framebuffer_t *framebuffer_open(void)
{
    IOStream *framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, OPEN_READ | OPEN_WRITE);

    if (framebuffer_device == NULL)
    {
        return NULL;
    }

    framebuffer_mode_arg_t mode_info;

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_GET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return NULL;
    }

    logger_info("Framebuffer create with graphic mode %dx%d.", mode_info.size.X, mode_info.size.Y);

    Bitmap *framebuffer_backbuffer = bitmap_create(mode_info.size.X, mode_info.size.Y);

    Painter *framebuffer_painter = painter_create(framebuffer_backbuffer);

    framebuffer_t *this = __create(framebuffer_t);

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
        .size = (Point){resx, resy},
    };

    if (iostream_call(this->device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) != 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return false;
    }

    painter_destroy(this->painter);
    bitmap_destroy(this->backbuffer);

    this->backbuffer = bitmap_create(resx, resy);
    this->painter = painter_create(this->backbuffer);

    this->width = mode_info.size.X;
    this->height = mode_info.size.Y;

    framebuffer_mark_dirty(this, bitmap_bound(this->backbuffer));

    return true;
}

Rectangle framebuffer_bound(framebuffer_t *this)
{
    return (Rectangle){{0, 0, this->width, this->height}};
}

void framebuffer_mark_dirty(framebuffer_t *this, Rectangle bound)
{
    if (rectange_colide(bitmap_bound(this->backbuffer), bound))
    {
        bound = rectangle_clip(bitmap_bound(this->backbuffer), bound);

        if (this->is_dirty)
        {
            this->dirty_bound = rectangle_merge(this->dirty_bound, bound);
        }
        else
        {
            this->is_dirty = true;
            this->dirty_bound = rectangle_clip(bitmap_bound(this->backbuffer), bound);
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

        args.buffer = this->backbuffer->pixels;
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

    args.buffer = this->backbuffer->pixels;
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
    painter_destroy(this->painter);
    bitmap_destroy(this->backbuffer);

    free(this);
}
#include <libgraphic/Framebuffer.h>
#include <libsystem/utils/Vector.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Framebuffer *_framebuffer;
static Painter *_painter;
static Bitmap *_wallpaper;

static Vector<Rectangle> _dirty_regions;

void renderer_initialize(void)
{
    _framebuffer = framebuffer_open();
    _painter = _framebuffer->painter;
    _wallpaper = bitmap_load_from("/System/Wallpapers/mountains.png");

    renderer_region_dirty(framebuffer_bound(_framebuffer));
}

void renderer_region_dirty(Rectangle new_region)
{
    bool merged = false;

    _dirty_regions.foreach ([&](Rectangle &region) {
        int region_area = region.area();
        int new_region_area = new_region.area();
        int merge_area = region.merged_with(new_region).area();

        if (region.colide_with(new_region) && (region_area + new_region_area > merge_area))
        {
            region = region.merged_with(new_region);
            merged = true;
        }

        return Iteration::CONTINUE;
    });

    if (!merged)
    {
        _dirty_regions.push_back(new_region);
    }
}

void renderer_region(Rectangle region)
{
    painter_blit_bitmap_no_alpha(_painter, _wallpaper, region, region);

    list_foreach_reversed(Window, window, manager_get_windows())
    {
        if (window_bound(window).colide_with(region))
        {
            Rectangle destination = window_bound(window).clipped_with(region);

            Rectangle source(
                destination.position() - window_bound(window).position(),
                destination.size());

            painter_blit_bitmap_no_alpha(_painter, window->frontbuffer, source, destination);
        }
    }

    framebuffer_mark_dirty(_framebuffer, region);
}

Rectangle renderer_bound(void)
{
    return framebuffer_bound(_framebuffer);
}

void renderer_repaint_dirty(void)
{
    _dirty_regions.foreach ([](Rectangle region) {
        renderer_region(region);

        if (region.colide_with(cursor_bound()))
        {
            renderer_region(cursor_bound());

            cursor_render(_painter);
        }

        return Iteration::CONTINUE;
    });

    framebuffer_blit_dirty(_framebuffer);

    _dirty_regions.clear();
}

void renderer_set_resolution(int width, int height)
{
    framebuffer_set_mode(_framebuffer, width, height);
    renderer_region_dirty(renderer_bound());
}

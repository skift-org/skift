#include <libgraphic/Framebuffer.h>
#include <libutils/Vector.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static OwnPtr<Framebuffer> _framebuffer;
static RefPtr<Bitmap> _wallpaper;

static Vector<Rectangle> _dirty_regions;

void renderer_initialize()
{
    _framebuffer = Framebuffer::open().take_value();
    _wallpaper = Bitmap::load_from_or_placeholder("/System/Wallpapers/mountains.png");

    renderer_region_dirty(_framebuffer->resolution());
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
    _framebuffer->painter().blit_bitmap_no_alpha(*_wallpaper, region, region);

    list_foreach_reversed(Window, window, manager_get_windows())
    {
        if (window->bound().colide_with(region))
        {
            Rectangle destination = window->bound().clipped_with(region);

            Rectangle source(
                destination.position() - window->bound().position(),
                destination.size());

            _framebuffer->painter().blit_bitmap_no_alpha(window->frontbuffer(), source, destination);
        }
    }

    _framebuffer->mark_dirty(region);
}

Rectangle renderer_bound()
{
    return _framebuffer->resolution();
}

void renderer_repaint_dirty()
{
    _dirty_regions.foreach ([](Rectangle region) {
        renderer_region(region);

        if (region.colide_with(cursor_bound()))
        {
            renderer_region(cursor_bound());

            cursor_render(_framebuffer->painter());
        }

        return Iteration::CONTINUE;
    });

    _framebuffer->blit();

    _dirty_regions.clear();
}

void renderer_set_resolution(int width, int height)
{
    _framebuffer->set_resolution(Vec2i(width, height));
    renderer_region_dirty(renderer_bound());
}

void renderer_set_wallaper(RefPtr<Bitmap> wallaper)
{
    _wallpaper = wallaper;
    renderer_region_dirty(renderer_bound());
}

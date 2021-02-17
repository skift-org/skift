#include <libgraphic/Framebuffer.h>
#include <libutils/Vector.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

#include "compositor/model/Wallpaper.h"

static OwnPtr<Framebuffer> _framebuffer;
static OwnPtr<compositor::Wallpaper> _wallpaper;

static Vector<Recti> _dirty_regions;

static OwnPtr<Settings::Setting> _night_light_enable_setting;
bool _night_light_enable = false;

static OwnPtr<Settings::Setting> _night_light_stenght_setting;
double _night_light_strenght = 0.5;

void renderer_initialize()
{
    _framebuffer = Framebuffer::open().take_value();
    _wallpaper = own<compositor::Wallpaper>(_framebuffer->resolution().size());
    _wallpaper->on_change = []() { renderer_region_dirty(renderer_bound()); };

    _night_light_enable_setting = own<Settings::Setting>("appearance:night-light.enable", [](auto &value) {
        _night_light_enable = value.as_bool();
        renderer_region_dirty(renderer_bound());
    });

    _night_light_stenght_setting = own<Settings::Setting>("appearance:night-light.stenght", [](auto &value) {
        _night_light_strenght = value.as_double();
        renderer_region_dirty(renderer_bound());
    });

    renderer_region_dirty(_framebuffer->resolution());
}

void renderer_region_dirty(Recti new_region)
{
    if (new_region.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_regions.foreach ([&](Recti &region) {
        if (region.colide_with(new_region))
        {
            Recti top;
            Recti botton;
            Recti left;
            Recti right;

            new_region.substract(region, top, botton, left, right);

            renderer_region_dirty(top);
            renderer_region_dirty(botton);
            renderer_region_dirty(left);
            renderer_region_dirty(right);

            merged = true;

            return Iteration::STOP;
        }
        else
        {
            return Iteration::CONTINUE;
        }
    });

    if (!merged)
    {
        _dirty_regions.push_back(new_region);
    }
}

void renderer_composite_wallpaper(Recti region)
{
    _framebuffer->painter().blit_no_alpha(_wallpaper->scaled(), region, region);
    _framebuffer->mark_dirty(region);
}

void renderer_composite_region(Recti region, Window *window_transparent)
{
    renderer_composite_wallpaper(region);

    manager_iterate_back_to_front([&](Window *window) {
        if (window == window_transparent)
        {
            return Iteration::STOP;
        }

        if (window->bound().colide_with(region))
        {
            Recti destination = window->bound().clipped_with(region);

            Recti source(
                destination.position() - window->bound().position(),
                destination.size());

            if (window->flags() & WINDOW_ACRYLIC)
            {
                _framebuffer->painter().blit_no_alpha(_wallpaper->acrylic(), destination, destination);
            }

            _framebuffer->painter().blit(window->frontbuffer(), source, destination);
        }

        return Iteration::CONTINUE;
    });

    _framebuffer->mark_dirty(region);
}

void renderer_region(Recti region)
{
    bool should_paint_wallpaper = true;

    if (region.is_empty())
    {
        return;
    }

    manager_iterate_front_to_back([&](Window *window) {
        if (window->bound().colide_with(region))
        {
            Recti destination = window->bound().clipped_with(region);

            Recti source(
                destination.position() - window->bound().position(),
                destination.size());

            int radius = 6;

            if (window->flags() & WINDOW_TRANSPARENT)
            {
                renderer_composite_region(destination, window);
                _framebuffer->painter().blit(window->frontbuffer(), source, destination);
            }
            else if (window->flags() & WINDOW_ACRYLIC)
            {
                if (window->flags() & WINDOW_NO_ROUNDED_CORNERS)
                {
                    _framebuffer->painter().blit_no_alpha(_wallpaper->acrylic(), region, region);
                    _framebuffer->painter().blit(window->frontbuffer(), source, destination);
                }
                else
                {
                    _framebuffer->painter().push();
                    _framebuffer->painter().clip(destination);

                    renderer_composite_region(window->bound().take_top_left(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_top_right(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_bottom_left(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_bottom_right(radius).clipped_with(region), window);

                    _framebuffer->painter().blit_rounded_no_alpha(_wallpaper->acrylic(), window->bound(), window->bound(), radius);
                    _framebuffer->painter().blit_rounded(window->frontbuffer(), window->bound().size(), window->bound(), radius);
                    _framebuffer->painter().pop();
                }
            }
            else
            {
                if (window->flags() & WINDOW_NO_ROUNDED_CORNERS)
                {
                    _framebuffer->painter().blit_no_alpha(window->frontbuffer(), source, destination);
                }
                else
                {
                    _framebuffer->painter().push();
                    _framebuffer->painter().clip(destination);

                    renderer_composite_region(window->bound().take_top_left(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_top_right(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_bottom_left(radius).clipped_with(region), window);
                    renderer_composite_region(window->bound().take_bottom_right(radius).clipped_with(region), window);

                    _framebuffer->painter().blit_rounded_no_alpha(window->frontbuffer(), window->bound().size(), window->bound(), radius);
                    _framebuffer->painter().pop();
                }
            }

            if (_night_light_enable)
            {
                _framebuffer->painter().sepia(destination, _night_light_strenght);
            }

            _framebuffer->mark_dirty(destination);

            Recti top;
            Recti botton;
            Recti left;
            Recti right;

            region.substract(destination, top, botton, left, right);

            renderer_region(top);
            renderer_region(botton);
            renderer_region(left);
            renderer_region(right);

            should_paint_wallpaper = false;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    if (should_paint_wallpaper)
    {
        renderer_composite_wallpaper(region);

        if (_night_light_enable)
        {
            _framebuffer->painter().sepia(region, _night_light_strenght);
        }
    }
}

Recti renderer_bound()
{
    return _framebuffer->resolution();
}

void renderer_repaint_dirty()
{
    _dirty_regions.foreach ([](Recti region) {
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

bool renderer_set_resolution(int width, int height)
{
    auto result = _framebuffer->set_resolution(Vec2i(width, height));

    if (result != SUCCESS)
    {
        return false;
    }

    _wallpaper->change_resolution({width, height});
    renderer_region_dirty(renderer_bound());

    return true;
}

void renderer_set_wallaper(RefPtr<Bitmap>)
{
    renderer_region_dirty(renderer_bound());
}

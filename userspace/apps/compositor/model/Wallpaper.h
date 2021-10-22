#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>

#include <libasync/Invoker.h>
#include <libsettings/Setting.h>

namespace compositor
{
struct Wallpaper
{
private:
    Graphic::BitmapScaling _scaling = Graphic::BitmapScaling::COVER;

    Math::Vec2i _resolution = {};
    Graphic::Color _background = Graphic::Colors::BLACK;
    Ref<Graphic::Bitmap> _orginal = nullptr;
    Ref<Graphic::Bitmap> _scaled = nullptr;
    Ref<Graphic::Bitmap> _acrylic = nullptr;

    Box<Settings::Setting> _setting_image;
    Box<Settings::Setting> _setting_color;
    Box<Settings::Setting> _setting_scaling;

    Box<Async::Invoker> _render_invoker;

public:
    Func<void()> on_change;

    Math::Recti bound() { return {{}, _resolution}; }

    int width() { return _resolution.x(); }
    int height() { return _resolution.y(); }

    Graphic::Bitmap &scaled()
    {
        return *_scaled;
    }

    Graphic::Bitmap &acrylic()
    {
        return *_acrylic;
    }

    Wallpaper(Math::Vec2i resolution);

    void render_scaled();

    void render_acrylic();

    void render();

    void change_resolution(Math::Vec2i resolution);
};

} // namespace compositor

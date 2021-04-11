#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>

#include <libsettings/Setting.h>
#include <libsystem/eventloop/Invoker.h>

namespace compositor
{
class Wallpaper
{
private:
    Graphic::BitmapScaling _scaling = Graphic::BitmapScaling::COVER;

    Math::Vec2i _resolution = {};
    Graphic::Color _background = Graphic::Colors::BLACK;
    RefPtr<Graphic::Bitmap> _orginal = nullptr;
    RefPtr<Graphic::Bitmap> _scaled = nullptr;
    RefPtr<Graphic::Bitmap> _acrylic = nullptr;

    OwnPtr<Settings::Setting> _setting_image;
    OwnPtr<Settings::Setting> _setting_color;
    OwnPtr<Settings::Setting> _setting_scaling;

    OwnPtr<Invoker> _render_invoker;

public:
    Callback<void()> on_change;

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

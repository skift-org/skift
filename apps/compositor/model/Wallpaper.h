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
    BitmapScaling _scaling = BitmapScaling::COVER;

    Vec2i _resolution = {};
    Color _background = Colors::BLACK;
    RefPtr<Bitmap> _orginal = nullptr;
    RefPtr<Bitmap> _scaled = nullptr;
    RefPtr<Bitmap> _acrylic = nullptr;

    OwnPtr<settings::Setting> _setting_image;
    OwnPtr<settings::Setting> _setting_color;
    OwnPtr<settings::Setting> _setting_scaling;

    OwnPtr<Invoker> _render_invoker;

public:
    Callback<void()> on_change;

    Recti bound() { return {{}, _resolution}; }

    int width() { return _resolution.x(); }
    int height() { return _resolution.y(); }

    Bitmap &scaled()
    {
        return *_scaled;
    }

    Bitmap &acrylic()
    {
        return *_acrylic;
    }

    Wallpaper(Vec2i resolution);

    void render_scaled();

    void render_acrylic();

    void render();

    void change_resolution(Vec2i resolution);
};

} // namespace compositor

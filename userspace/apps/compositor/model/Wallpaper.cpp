#include "compositor/model/Wallpaper.h"

namespace compositor
{

Wallpaper::Wallpaper(Math::Vec2i resolution)
    : _resolution(resolution),
      _scaled(Graphic::Bitmap::create_shared(resolution.x(), resolution.y()).unwrap()),
      _acrylic(Graphic::Bitmap::create_shared(resolution.x(), resolution.y()).unwrap())
{
    _render_invoker = own<Invoker>([this]() {
        render();
    });

    _setting_image = own<Settings::Setting>("appearance:wallpaper.image", [this](auto &value) {
        if (value.is(Json::STRING))
        {
            _orginal = Graphic::Bitmap::load_from_or_placeholder(value.as_string());
        }
        else
        {
            _orginal = nullptr;
        }

        _render_invoker->invoke_later();
    });

    _setting_color = own<Settings::Setting>("appearance:wallpaper.color", [this](auto &value) {
        if (value.is(Json::STRING))
        {
            _background = Graphic::Color::parse(value.as_string());
        }
        else
        {
            _background = Graphic::Colors::MAGENTA;
        }

        _render_invoker->invoke_later();
    });

    _setting_scaling = own<Settings::Setting>("appearance:wallpaper.scaling", [this](auto &value) {
        auto scaling_name = value.as_string();

        if (scaling_name == "center")
        {
            _scaling = Graphic::BitmapScaling::CENTER;
        }
        else if (scaling_name == "stretch")
        {
            _scaling = Graphic::BitmapScaling::STRETCH;
        }
        else if (scaling_name == "cover")
        {
            _scaling = Graphic::BitmapScaling::COVER;
        }
        else if (scaling_name == "fit")
        {
            _scaling = Graphic::BitmapScaling::FIT;
        }

        _render_invoker->invoke_later();
    });

    render();
}

void Wallpaper::render_scaled()
{
    Graphic::Painter painter{*_scaled};

    painter.clear(_background);

    if (_orginal)
    {
        painter.blit(*_orginal, _scaling, _scaled->bound());
    }
}

void Wallpaper::render_acrylic()
{
    _acrylic->copy_from(*_scaled, _scaled->bound());
    Graphic::Painter painter(*_acrylic);

    painter.acrylic(_acrylic->bound());
}

void Wallpaper::render()
{
    render_scaled();
    render_acrylic();

    if (on_change)
    {
        on_change();
    }
}

void Wallpaper::change_resolution(Math::Vec2i resolution)
{
    _resolution = resolution;
    _scaled = Graphic::Bitmap::create_shared(_resolution.x(), _resolution.y()).unwrap();
    _acrylic = Graphic::Bitmap::create_shared(_resolution.x(), _resolution.y()).unwrap();

    _render_invoker->invoke_later();
}

} // namespace compositor

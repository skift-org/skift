#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>

#include <libsettings/Setting.h>
#include <libsystem/eventloop/Invoker.h>

namespace compositor
{

class Wallpaper
{
public:
    enum Scaling
    {
        CENTER,
        STRETCH,
        COVER,
        FIT,
    };

private:
    Scaling _scaling = COVER;

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

    Wallpaper(Vec2i resolution)
        : _resolution(resolution),
          _scaled(Bitmap::create_shared(resolution.x(), resolution.y()).value()),
          _acrylic(Bitmap::create_shared(resolution.x(), resolution.y()).value())
    {
        _render_invoker = own<Invoker>([this]() {
            render();
        });

        _setting_image = own<settings::Setting>("appearance:wallpaper.image", [this](auto &value) {
            if (value.is(json::STRING))
            {
                _orginal = Bitmap::load_from_or_placeholder(value.as_string().cstring());
            }
            else
            {
                _orginal = nullptr;
            }

            _render_invoker->invoke_later();
        });

        _setting_color = own<settings::Setting>("appearance:wallpaper.color", [this](auto &value) {
            if (value.is(json::STRING))
            {
                _background = Color::parse(value.as_string().cstring());
            }
            else
            {
                _background = Colors::MAGENTA;
            }

            _render_invoker->invoke_later();
        });

        _setting_scaling = own<settings::Setting>("appearance:wallpaper.scaling", [this](auto &value) {
            auto scaling_name = value.as_string();

            if (scaling_name == "center")
            {
                _scaling = CENTER;
            }
            else if (scaling_name == "stretch")
            {
                _scaling = STRETCH;
            }
            else if (scaling_name == "cover")
            {
                _scaling = COVER;
            }
            else if (scaling_name == "fit")
            {
                _scaling = FIT;
            }

            _render_invoker->invoke_later();
        });

        render();
    }

    ~Wallpaper()
    {
    }

    void render_scaled()
    {
        Painter painter{*_scaled};

        painter.clear(_background);

        if (_orginal)
        {
            if (_scaling == COVER)
            {
                painter.blit(*_orginal, _orginal->bound(), _orginal->bound().cover(_scaled->bound()));
            }
            else if (_scaling == STRETCH)
            {
                painter.blit(*_orginal, _orginal->bound(), _scaled->bound());
            }
            else if (_scaling == CENTER)
            {
                painter.blit(*_orginal, _orginal->bound(), _orginal->bound().centered_within(_scaled->bound()));
            }
            else if (_scaling == FIT)
            {
                painter.blit(*_orginal, _orginal->bound(), _orginal->bound().fit(_scaled->bound()));
            }
        }
    }

    void render_acrylic()
    {
        _acrylic->copy_from(*_scaled, _scaled->bound());
        Painter painter(*_acrylic);

        painter.acrylic(_acrylic->bound());
    }

    void render()
    {
        render_scaled();
        render_acrylic();

        renderer_region_dirty(renderer_bound());
    }

    void change_resolution(Vec2i resolution)
    {
        _resolution = resolution;
        _scaled = Bitmap::create_shared(_resolution.x(), _resolution.y()).value();
        _acrylic = Bitmap::create_shared(_resolution.x(), _resolution.y()).value();

        _render_invoker->invoke_later();
    }
};

} // namespace compositor

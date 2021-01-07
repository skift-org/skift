#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libutils/Observable.h>

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
    };

private:
    Scaling _scaling = COVER;

    Vec2i _resolution = {};
    Color _background = Colors::BLACK;
    RefPtr<Bitmap> _orginal = nullptr;
    RefPtr<Bitmap> _scaled = nullptr;
    RefPtr<Bitmap> _acrylic = nullptr;

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
                painter.blit_bitmap(*_orginal, _orginal->bound(), _orginal->bound().cover(_scaled->bound()));
            }
            else if (_scaling == STRETCH)
            {
                painter.blit_bitmap(*_orginal, _orginal->bound(), _scaled->bound());
            }
            else if (_scaling == COVER)
            {
                painter.blit_bitmap(*_orginal, _orginal->bound(), _orginal->bound().centered_within(_scaled->bound()));
            }
        }
    }

    void render_acrylic()
    {
        _acrylic->copy_from(*_scaled, _scaled->bound());
        Painter painter(*_acrylic);

        painter.saturation(_acrylic->bound(), 0.25);
        painter.blur_rectangle(_acrylic->bound(), 30);
        painter.noise(_acrylic->bound(), 0.05);
    }

    void render()
    {
        render_scaled();
        render_acrylic();
    }

    void change_resolution(Vec2i resolution)
    {
        _resolution = resolution;
        _scaled = Bitmap::create_shared(_resolution.x(), _resolution.y()).value();
        _acrylic = Bitmap::create_shared(_resolution.x(), _resolution.y()).value();
        render();
    }

    void change_image(RefPtr<Bitmap> image)
    {
        _orginal = image;
        render();
    }

    void change_background(Color color)
    {
        _background = color;
        render();
    }

    void change_scaling(Scaling scaling)
    {
        _scaling = scaling;
        render();
    }
}; // namespace compositor

} // namespace compositor

#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct ScrollBarElement : public Element
{
private:
    Math::Vec2i _mouse_origin;

    int _track = 0;
    int _thumb = 0;
    int _value = 0;

    bool _mouse_over = false;
    bool _mouse_drag = false;

    bool _horizontal = false;

    Math::Recti track_bound()
    {
        return bound().shrinked(Insetsi(4, 4, 4));
    }

    Math::Recti thumb_bound()
    {
        Math::Recti track = track_bound();

        if (_horizontal)
        {
            int thumb_width = MIN(track.width() * (_thumb / (float)_track), track.width());
            int thump_position = track.width() * (_value / (float)_track);

            return {
                track.x() + thump_position,
                track.y(),
                thumb_width,
                track.height(),
            };
        }
        else
        {
            int thumb_height = MIN(track.height() * (_thumb / (float)_track), track.height());
            int thump_position = track.height() * (_value / (float)_track);

            return {
                track.x(),
                track.y() + thump_position,
                track.width(),
                thumb_height,
            };
        }
    }

public:
    static constexpr int SIZE = 16;

    ScrollBarElement(bool horizontal = false);

    void scroll_to(Math::Vec2i mouse_position);

    int value() { return _value; }

    void value(int value) { _value = clamp(value, 0, _track - _thumb); }

    void update(int content, int view)
    {
        update(content, view, _value);
    }

    void update(int content, int view, int value)
    {
        _track = content;
        _thumb = view;
        this->value(value);

        should_repaint();
    }

    void paint(Graphic::Painter &, const Math::Recti &) override;

    void event(Event *event) override;

    virtual Math::Vec2i size() override
    {
        return {SIZE, SIZE};
    }
};

WIDGET_BUILDER(ScrollBarElement, scrollbar)

} // namespace Widget

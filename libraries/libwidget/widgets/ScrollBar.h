#pragma once

#include <libwidget/Widget.h>

class ScrollBar : public Widget
{
private:
    Vec2i _mouse_origine;

    int _track = 0;
    int _thumb = 0;
    int _value = 0;

    bool _mouse_over;
    bool _horizontal = false;

    Rectangle track_bound()
    {
        return bound().shrinked(Insets(4, 4, 4));
    }

    Rectangle thumb_bound()
    {
        Rectangle track = track_bound();

        if (_horizontal)
        {
            int thumb_width = MIN(track.width() * (_thumb / (float)_track), track.width());
            int thump_position = track.width() * (_value / (float)_track);

            return Rectangle(
                track.x() + thump_position,
                track.y(),
                thumb_width,
                track.height());
        }
        else
        {
            int thumb_height = MIN(track.height() * (_thumb / (float)_track), track.height());
            int thump_position = track.height() * (_value / (float)_track);

            return Rectangle(
                track.x(),
                track.y() + thump_position,
                track.width(),
                thumb_height);
        }
    }

public:
    static constexpr int SIZE = 16;

    void horizontal(bool value)
    {
        _horizontal = value;
    }

    ScrollBar(Widget *parent);

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void scroll_to(Vec2i mouse_position);

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
};

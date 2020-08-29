#pragma once

#include <libutils/String.h>
#include <libwidget/Widget.h>

class Label : public Widget
{
private:
    String _text = "Label";
    Position _position = Position::LEFT;

public:
    void text(String text)
    {
        _text = text;
        should_repaint();
    }

    Label(Widget *parent, String text);

    Label(Widget *parent, String text, Position position);

    void paint(Painter &painter, Rectangle rectangle);

    Vec2i size();
};

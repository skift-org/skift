#pragma once

#include <libwidget/Widget.h>

struct Label : public Widget
{
private:
    char *_text = nullptr;
    Position _position = Position::LEFT;

public:
    void text(const char *text);

    Label(Widget *parent, const char *text);

    Label(Widget *parent, const char *text, Position position);

    ~Label();

    void paint(Painter &painter, Rectangle rectangle);

    Vec2i size();
};

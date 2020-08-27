#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

enum ButtonState
{
    BUTTON_IDLE,
    BUTTON_OVER,
    BUTTON_PRESS,
};

enum ButtonStyle
{
    BUTTON_TEXT,
    BUTTON_OUTLINE,
    BUTTON_FILLED,
};

class Button : public Widget
{
private:
    ButtonState _state = BUTTON_IDLE;
    ButtonStyle _style;

public:
    Button(Widget *parent, ButtonStyle style);

    Button(Widget *parent, ButtonStyle style, RefPtr<Icon> icon);

    Button(Widget *parent, ButtonStyle style, String text);

    Button(Widget *parent, ButtonStyle style, RefPtr<Icon> icon, String text);

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);
};

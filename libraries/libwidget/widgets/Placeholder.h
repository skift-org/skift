#pragma once

#include <libwidget/Widget.h>

class Placeholder : public Widget
{
private:
    char *_text;
    RefPtr<Icon> _alert_icon;

public:
    Placeholder(Widget *parent, const char *text);

    ~Placeholder();

    void paint(Painter &painter, Rectangle rectangle);
};

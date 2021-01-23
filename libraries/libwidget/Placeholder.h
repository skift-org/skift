#pragma once

#include <libwidget/Widget.h>

class Placeholder : public Widget
{
private:
    String _text;
    RefPtr<Icon> _alert_icon;

public:
    Placeholder(Widget *parent, String text);

    void paint(Painter &, const Recti &) override;
};

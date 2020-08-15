#pragma once

#include <libwidget/Widget.h>
#include <libwidget/widgets/ScrollBar.h>

class TextField : public Widget
{
private:
    ScrollBar *_scrollbar;

public:
    TextField(Widget *parent);

    ~TextField();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};

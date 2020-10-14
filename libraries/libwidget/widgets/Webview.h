#pragma once

#include <libwidget/Widget.h>

class Webview : public Widget
{
private:
    char* webview_copy_data;
public:
    Webview(Widget *parent, const char* data);

    ~Webview();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);
};

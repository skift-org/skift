#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct Status : public Widget::Window
{
private:
    Box<Widget::Window> _menu;
    Box<Widget::Window> _datetime;
    Box<Widget::Window> _quicksetting;

public:
    Status();

    Ref<Widget::Element> build() override;
};

} // namespace Panel

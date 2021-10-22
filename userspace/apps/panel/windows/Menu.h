#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct Menu :
    public Widget::Window
{
    Menu();
    Ref<Widget::Element> build() override;
};

} // namespace Panel

#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct Menu :
    public Widget::Window
{
    Menu();
    RefPtr<Widget::Element> build() override;
};

} // namespace Panel

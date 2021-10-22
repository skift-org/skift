#pragma once

#include <libwidget/Window.h>

namespace Snake
{

struct Game : public Widget::Window
{
    Ref<Widget::Element> build();
};

} // namespace Snake

#pragma once

#include <libwidget/Window.h>

namespace Snake
{

struct Game : public Widget::Window
{
    RefPtr<Widget::Element> build();
};

} // namespace Snake

#pragma once

#include <libwidget/Window.h>

namespace panel
{

class QuickSettingsWindow : public Widget::Window
{
public:
    static constexpr int WIDTH = 320;
    static constexpr int HEIGHT = 320;

    QuickSettingsWindow();
};

} // namespace panel

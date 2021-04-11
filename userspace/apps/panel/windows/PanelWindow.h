#pragma once

#include <libwidget/Window.h>

#include "panel/windows/DateAndTimeWindow.h"
#include "panel/windows/MenuWindow.h"
#include "panel/windows/QuickSettingsWindow.h"

namespace panel
{

class PanelWindow : public Widget::Window
{
private:
    OwnPtr<MenuWindow> _menu;
    OwnPtr<DateAndTimeWindow> _datetime;
    OwnPtr<QuickSettingsWindow> _quicksetting;

public:
    static constexpr int HEIGHT = 38;

    PanelWindow();
};

} // namespace panel

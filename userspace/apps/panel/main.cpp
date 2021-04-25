#include <libwidget/Application.h>

#include "panel/windows/PanelWindow.h"

static constexpr int PANEL_HEIGHT = 38;

int main(int, char **)
{
    auto window = own<panel::PanelWindow>();
    window->show();
    return Widget::Application::the()->run();
}

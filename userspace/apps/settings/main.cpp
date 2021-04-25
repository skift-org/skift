#include <libwidget/Application.h>

#include "settings/windows/MainWindow.h"

int main(int, char **)
{
    auto window = new Settings::MainWindow();
    window->show();
    return Widget::Application::the()->run();
}

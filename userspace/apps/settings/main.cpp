#include <libwidget/Application.h>

#include "settings/windows/MainWindow.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto window = new Settings::MainWindow();
    window->show();

    return Widget::Application::run();
}

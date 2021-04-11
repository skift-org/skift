#include <libwidget/Application.h>

#include "snake/windows/MainWindow.h"

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Widget::Application::initialize(argc, argv);

    auto window = own<Snake::MainWindow>();
    window->show();

    return Widget::Application::run();
}

#include <libwidget/Application.h>

#include "neko/windows/MainWindow.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto window = new neko::MainWindow();
    window->show();

    return Widget::Application::run();
}

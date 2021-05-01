#include <libwidget/Application.h>

#include "neko/windows/MainWindow.h"

int main(int, char **)
{
    auto window = new neko::MainWindow();
    window->show();

    return Widget::Application::the().run();
}

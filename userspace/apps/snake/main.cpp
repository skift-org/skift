#include <libwidget/Application.h>

#include "snake/windows/MainWindow.h"

int main(int, char **)
{
    auto window = own<Snake::MainWindow>();
    window->show();
    return Widget::Application::the()->run();
}

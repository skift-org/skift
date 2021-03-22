#include <libwidget/Application.h>

#include "snake/windows/MainWindow.h"

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    Application::initialize(argc, argv);

    auto window = own<Snake::MainWindow>();
    window->show();

    return Application::run();
}

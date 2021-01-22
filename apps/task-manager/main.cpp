#include <libwidget/Application.h>

#include "task-manager/windows/MainWindow.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto window = new task_manager::MainWinow();
    window->show();

    return Application::run();
}

#include <libwidget/Application.h>

#include "task-manager/windows/MainWindow.h"

int main(int, char **)
{
    auto window = new task_manager::MainWinow();
    window->show();
    return Widget::Application::the()->run();
}

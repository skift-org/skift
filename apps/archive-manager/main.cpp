#include <libsystem/process/Process.h>
#include <libwidget/Application.h>

#include "archive-manager/MainWindow.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto window = new MainWindow();

    window->show();

    return Application::run();
}

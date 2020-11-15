#include <libwidget/Application.h>

#include "neko/Window.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new neko::Window();
    window->show();

    return application_run();
}

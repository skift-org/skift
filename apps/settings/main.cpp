#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = window_create_from_file("/Applications/settings/settings.markup");
    window->show();

    return Application::run();
}

#include <libwidget/Application.h>
#include <libwidget/Markup.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create_from_file("/Applications/media-player/main.markup");

    window->show();

    return application_run();
}

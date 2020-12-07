#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

#include "media-player/windows/Main.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new media_player::Main();

    window->show();

    return application_run();
}

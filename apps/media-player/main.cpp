#include <libwidget/Application.h>
#include <libwidget/Markup.h>

#include "media-player/windows/Main.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto window = new media_player::Main();

    window->show();

    return Application::run();
}

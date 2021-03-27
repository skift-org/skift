#include <libwidget/Application.h>

#include "media-player/windows/Main.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto window = new MediaPlayer::Main();

    window->show();

    return Application::run();
}

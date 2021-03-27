#include <libwidget/Application.h>

#include "media-player/windows/Main.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto window = new MediaPlayer::Main();

    window->show();

    return Widget::Application::run();
}

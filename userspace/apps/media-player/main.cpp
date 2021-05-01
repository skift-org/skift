#include <libwidget/Application.h>

#include "media-player/windows/Main.h"

int main(int, char **)
{
    auto window = new MediaPlayer::Main();

    window->show();

    return Widget::Application::the().run();
}

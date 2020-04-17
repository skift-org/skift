#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("/res/icon/info.png", "About", 250, 250, WINDOW_NONE);

    window_root(window)->layout = (Layout){LAYOUT_VGRID, 0, 8};

    image_create(window_root(window), "/res/skift.png");

    Widget *button_and_text = container_create(window_root(window));
    button_and_text->layout = (Layout){LAYOUT_VGRID, 0, 2};

    label_create(button_and_text, "The skift operating system.");
    label_create(button_and_text, __BUILD_GITREF__ "/" __BUILD_CONFIG__);
    label_create(button_and_text, "Copyright (c) 2018-2020 N. Van Bossuyt.");

    button_create(button_and_text, "Ok");

    return application_run();
}

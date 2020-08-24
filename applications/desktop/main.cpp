#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED);

    window->title("Desktop");
    window->type(WINDOW_TYPE_DESKTOP);
    window->bound(screen_get_bound());
    window->on(Event::DISPLAY_SIZE_CHANGED, [&](auto event) {
        window->bound(window_bound(window).resized(event->display.size));
    });

    window_root(window)->layout(STACK());

    if (argc == 2)
    {
        auto wallpaper = Bitmap::load_from_or_placeholder(argv[1]);
        auto background = new Image(window_root(window), wallpaper);
        background->change_scaling(ImageScalling::STRETCH);
    }

    window_show(window);

    return application_run();
}

#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED);

    window->title("Desktop");
    window->type(WINDOW_TYPE_DESKTOP);
    window->bound(Screen::bound());
    window->on(Event::DISPLAY_SIZE_CHANGED, [&](auto) {
        window->bound(Screen::bound());
    });

    window->root()->layout(STACK());

    if (argc == 2)
    {
        auto wallpaper = Bitmap::load_from_or_placeholder(argv[1]);
        auto background = new Image(window->root(), wallpaper);
        background->change_scaling(ImageScalling::STRETCH);
    }

    window->show();

    return application_run();
}

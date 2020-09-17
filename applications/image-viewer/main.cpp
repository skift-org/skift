#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    if (argc == 1)
        return -1;

    if (application_initialize(argc, argv) != SUCCESS)
        return -1;

    Window *window = new Window(WINDOW_RESIZABLE);

    window->icon(Icon::get("image"));
    window->title("Image Viewer");
    window->size(Vec2i(700, 500));

    new Image(window->root(), Bitmap::load_from_or_placeholder(argv[1]));

    window->show();

    return application_run();
}

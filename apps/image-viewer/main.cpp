#include <libsettings/Settings.h>

#include <libsystem/process/Process.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Image.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>

int main(int argc, char **argv)
{
    if (argc == 1)
        return -1;

    if (Application::initialize(argc, argv) != SUCCESS)
        return -1;

    Window *window = new Window(WINDOW_RESIZABLE);

    window->icon(Icon::get("image"));
    window->title("Image Viewer");
    window->size(Vec2i(700, 500));
    window->root()->layout(VFLOW(0));

    new TitleBar(window->root());

    auto toolbar = new Panel(window->root());
    toolbar->layout(HFLOW(0));
    toolbar->insets(4);

    auto bitmap = Bitmap::load_from_or_placeholder(argv[1]);

    auto set_has_wallaper = new Button(toolbar, Button::TEXT, Icon::get("wallpaper"), "Set As Wallpaper");

    set_has_wallaper->on(Event::ACTION, [&](auto) {
        Settings::write(Settings::Path::parse("appearance:wallpaper.image"), process_resolve(argv[1]));
    });

    auto image = new Image(window->root(), bitmap);
    image->flags(Widget::FILL);

    window->show();

    return Application::run();
}

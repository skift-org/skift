#include <libsettings/Service.h>

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

    if (Widget::Application::initialize(argc, argv) != SUCCESS)
        return -1;

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);

    window->icon(Graphic::Icon::get("image"));
    window->title("Image Viewer");
    window->size(Math::Vec2i(700, 500));
    window->root()->layout(VFLOW(0));

    new Widget::TitleBar(window->root());

    auto toolbar = new Widget::Panel(window->root());
    toolbar->layout(HFLOW(0));
    toolbar->insets(4);

    auto bitmap = Graphic::Bitmap::load_from_or_placeholder(argv[1]);

    auto set_has_wallaper = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("wallpaper"), "Set As Wallpaper");

    set_has_wallaper->on(Widget::Event::ACTION, [&](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:wallpaper.image"), process_resolve(argv[1]));
    });

    auto image = new Widget::Image(window->root(), bitmap);
    image->flags(Widget::Component::FILL);

    window->show();

    return Widget::Application::run();
}

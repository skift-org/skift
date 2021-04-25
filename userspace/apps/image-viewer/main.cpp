#include <libsettings/Service.h>

#include <libsystem/process/Process.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/TitleBar.h>

int main(int argc, char **argv)
{
    if (argc == 1)
        return -1;

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);

    window->size(Math::Vec2i(700, 500));
    window->root()->layout(VFLOW(0));

    window->root()->add<Widget::TitleBar>(
        Graphic::Icon::get("image"),
        "Image Viewer");

    auto toolbar = window->root()->add(Widget::panel());
    toolbar->layout(HFLOW(0));
    toolbar->insets(4);

    auto bitmap = Graphic::Bitmap::load_from_or_placeholder(argv[1]);

    auto set_has_wallaper = toolbar->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("wallpaper"), "Set As Wallpaper");

    set_has_wallaper->on(Widget::Event::ACTION, [&](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:wallpaper.image"), process_resolve(argv[1]));
    });

    auto image = window->root()->add(Widget::image(bitmap));
    image->flags(Widget::Element::FILL);

    window->show();

    return Widget::Application::the()->run();
}

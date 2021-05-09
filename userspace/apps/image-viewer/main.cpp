#include <libsettings/Service.h>

#include <libsystem/process/Process.h>

#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>

int main(int argc, char **argv)
{
    if (argc == 1)
        return -1;

    Widget::Window *window = new Widget::Window(WINDOW_RESIZABLE);

    window->size(Math::Vec2i(700, 500));

    window->root()->add(Widget::titlebar(Graphic::Icon::get("image"), "Image Viewer"));

    auto toolbar = window->root()->add(Widget::panel());

    auto bitmap = Graphic::Bitmap::load_from_or_placeholder(argv[1]);

    auto set_has_wallaper = toolbar->add(Widget::basic_button(Graphic::Icon::get("wallpaper"), "Set As Wallpaper", [&] {
        Settings::Service::the()->write(Settings::Path::parse("appearance:wallpaper.image"), process_resolve(argv[1]));
    }));

    auto image = window->root()->add(Widget::image(bitmap));
    image->flags(Widget::Element::FILL);

    window->show();

    return Widget::Application::the().run();
}

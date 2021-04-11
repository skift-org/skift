#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>
#include <libwidget/Spacer.h>
#include <libwidget/TextEditor.h>
#include <libwidget/TitleBar.h>

static auto logo_based_on_color_scheme()
{
    auto path = Widget::theme_is_dark() ? "/Applications/about/logo-white.png"
                                        : "/Applications/about/logo-black.png";

    return Graphic::Bitmap::load_from_or_placeholder(path);
}

void show_license()
{
    auto license_window = new Widget::Window(WINDOW_NONE);
    license_window->title("License");
    license_window->size({570, 416});
    license_window->root()->layout(VFLOW(0));

    new Widget::TitleBar(license_window->root());

    auto field = new Widget::TextEditor(license_window->root(), Widget::TextModel::from_file("/Files/license.md"));
    field->flags(Widget::Component::FILL);
    field->readonly(true);
    field->font(Graphic::Font::get("mono").unwrap());
    field->focus();

    license_window->show();
}

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    Widget::Window *window = new Widget::Window(WINDOW_NONE);
    window->title("about");
    window->icon(Graphic::Icon::get("information"));
    window->root()->layout(VFLOW(4));

    new Widget::TitleBar(window->root());

    auto content_container = new Widget::Container(window->root());
    content_container->layout(VFLOW(4));
    content_container->insets(6);
    {

        auto system_logo = new Widget::Image(content_container, logo_based_on_color_scheme());
        system_logo->scaling(Graphic::BitmapScaling::CENTER);
        system_logo->outsets({32, 64});

        new Widget::Label(content_container, "The skift operating system.", Anchor::CENTER);
        new Widget::Label(content_container, __BUILD_VERSION__, Anchor::CENTER);
        new Widget::Label(content_container, __BUILD_GITREF__ "/" __BUILD_CONFIG__, Anchor::CENTER);
        new Widget::Label(content_container, "Copyright © 2018-2021", Anchor::CENTER);
        new Widget::Label(content_container, "N. Van Bossuyt & contributors.", Anchor::CENTER);
    }

    auto buttons_container = new Widget::Container(window->root());
    buttons_container->layout(HFLOW(0));
    buttons_container->insets(6);
    {
        auto license_button = new Widget::Button(buttons_container, Widget::Button::OUTLINE, "License");
        license_button->on(Widget::Event::ACTION, [window](auto) {
            show_license();
        });

        new Widget::Spacer(buttons_container);

        auto ok_button = new Widget::Button(buttons_container, Widget::Button::FILLED, "Ok");
        ok_button->on(Widget::Event::ACTION, [window](auto) {
            window->hide();
        });
    }

    window->resize_to_content();
    window->show();

    return Widget::Application::run();
}

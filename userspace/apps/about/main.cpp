#include "libgraphic/Icon.h"
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
    license_window->size({570, 416});
    license_window->root()->layout(VFLOW(0));

    license_window->root()->add<Widget::TitleBar>(
        Graphic::Icon::get("information"),
        "License");

    auto field = license_window->root()->add<Widget::TextEditor>(Widget::TextModel::open("/Files/license.md"));
    field->flags(Widget::Element::FILL);
    field->readonly(true);
    field->font(Graphic::Font::get("mono").unwrap());
    field->focus();

    license_window->show();
}

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    Widget::Window *window = new Widget::Window(WINDOW_NONE);
    window->root()->layout(VFLOW(4));

    window->root()->add<Widget::TitleBar>(
        Graphic::Icon::get("information"),
        "About");

    auto content_container = window->root()->add<Widget::Container>();
    content_container->layout(VFLOW(4));
    content_container->insets(6);
    {

        auto system_logo = content_container->add<Widget::Image>(logo_based_on_color_scheme());
        system_logo->scaling(Graphic::BitmapScaling::CENTER);
        system_logo->outsets({32, 64});

        content_container->add<Widget::Label>("The skift operating system.", Anchor::CENTER);
        content_container->add<Widget::Label>(__BUILD_VERSION__, Anchor::CENTER);
        content_container->add<Widget::Label>(__BUILD_GITREF__ "/" __BUILD_CONFIG__, Anchor::CENTER);
        content_container->add<Widget::Label>("Copyright © 2018-2021", Anchor::CENTER);
        content_container->add<Widget::Label>("N. Van Bossuyt & contributors.", Anchor::CENTER);
    }

    auto buttons_container = window->root()->add<Widget::Container>();
    buttons_container->layout(HFLOW(0));
    buttons_container->insets(6);
    {
        auto license_button = buttons_container->add<Widget::Button>(Widget::Button::OUTLINE, "License");
        license_button->on(Widget::Event::ACTION, [window](auto) {
            show_license();
        });

        buttons_container->add<Widget::Spacer>();

        auto ok_button = buttons_container->add<Widget::Button>(Widget::Button::FILLED, "Ok");
        ok_button->on(Widget::Event::ACTION, [window](auto) {
            window->hide();
        });
    }

    window->resize_to_content();
    window->show();

    return Widget::Application::run();
}

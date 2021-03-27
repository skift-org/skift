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
    auto path = theme_is_dark() ? "/Applications/about/logo-white.png"
                                : "/Applications/about/logo-black.png";

    return Graphic::Bitmap::load_from_or_placeholder(path);
}

void show_license()
{
    auto license_window = new Window(WINDOW_NONE);
    license_window->title("License");
    license_window->size({570, 416});
    license_window->root()->layout(VFLOW(0));

    new TitleBar(license_window->root());

    auto field = new TextEditor(license_window->root(), TextModel::from_file("/Files/license.md"));
    field->flags(Component::FILL);
    field->readonly(true);
    field->font(Graphic::Font::get("mono").take_value());
    field->focus();

    license_window->show();
}

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = new Window(WINDOW_NONE);
    window->title("about");
    window->icon(Graphic::Icon::get("information"));
    window->root()->layout(VFLOW(4));

    new TitleBar(window->root());

    auto content_container = new Container(window->root());
    content_container->layout(VFLOW(4));
    content_container->insets(6);
    {

        auto system_logo = new Image(content_container, logo_based_on_color_scheme());
        system_logo->scaling(Graphic::BitmapScaling::CENTER);
        system_logo->outsets({32, 64});

        new Label(content_container, "The skift operating system.", Anchor::CENTER);
        new Label(content_container, __BUILD_VERSION__, Anchor::CENTER);
        new Label(content_container, __BUILD_GITREF__ "/" __BUILD_CONFIG__, Anchor::CENTER);
        new Label(content_container, "Copyright © 2018-2021", Anchor::CENTER);
        new Label(content_container, "N. Van Bossuyt & contributors.", Anchor::CENTER);
    }

    auto buttons_container = new Container(window->root());
    buttons_container->layout(HFLOW(0));
    buttons_container->insets(6);
    {
        auto license_button = new Button(buttons_container, Button::OUTLINE, "License");
        license_button->on(Event::ACTION, [window](auto) {
            show_license();
        });

        new Spacer(buttons_container);

        auto ok_button = new Button(buttons_container, Button::FILLED, "Ok");
        ok_button->on(Event::ACTION, [window](auto) {
            window->hide();
        });
    }

    window->resize_to_content();
    window->show();

    return Application::run();
}

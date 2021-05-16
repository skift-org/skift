#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Views.h>

using namespace Widget;

static auto logo_based_on_color_scheme()
{
    auto path = Widget::theme_is_dark()
                    ? "/Applications/about/logo-white.png"
                    : "/Applications/about/logo-black.png";

    return Graphic::Bitmap::load_from_or_placeholder(path);
}

static void show_license()
{

    auto texteditor = Widget::texteditor(Widget::TextModel::open("/Files/license.md"));
    texteditor->readonly(true);
    texteditor->font(Graphic::Font::get("mono").unwrap());
    texteditor->focus();

    // clang-format off

    auto license_window = window(
        vflow({
            titlebar(Graphic::Icon::get("information"), "License"),
            fill(texteditor),
        })
    );

    license_window->show();

    // clang-format on
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Application app;

    // clang-format off

    auto win = window(
        vflow(4, {
            titlebar(Graphic::Icon::get("information"), "About"),

            spacing(6,
                vflow(4,{
                    spacing(
                        {32, 64},
                        image(logo_based_on_color_scheme(), Graphic::BitmapScaling::CENTER)
                    ),
                    label("The skift operating system.", Anchor::CENTER),
                    label(__BUILD_VERSION__, Anchor::CENTER),
                    label(__BUILD_GITREF__ "/" __BUILD_CONFIG__, Anchor::CENTER),
                    label("Copyright © 2018-2021", Anchor::CENTER),
                    label("N. Van Bossuyt & contributors.", Anchor::CENTER),
                })
            ),

            spacing(6,
                hflow(4,{
                    outline_button("License",  [] {
                        show_license();
                    }),
                    spacer(),
                    filled_button("Ok", [&] {
                        app.exit(PROCESS_SUCCESS);
                    })
                })
            )
        })
    );

    // clang-format on

    win->show();

    return app.run();
}

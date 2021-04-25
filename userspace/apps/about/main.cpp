#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>
#include <libwidget/Spacer.h>
#include <libwidget/TextEditor.h>
#include <libwidget/TitleBar.h>

#include <libwidget/Layouts.h>

static auto logo_based_on_color_scheme()
{
    auto path = Widget::theme_is_dark() ? "/Applications/about/logo-white.png"
                                        : "/Applications/about/logo-black.png";

    return Graphic::Bitmap::load_from_or_placeholder(path);
}

void show_license()
{
    using namespace Widget;

    auto texteditor = Widget::texteditor(Widget::TextModel::open("/Files/license.md"));
    texteditor->readonly(true);
    texteditor->font(Graphic::Font::get("mono").unwrap());
    texteditor->focus();

    // clang-format off

    auto license_window = window(
        vflow(0, {
            titlebar(Graphic::Icon::get("information"), "License"),
            fill(texteditor),
        })
    );

    license_window->show();

    // clang-format on
}

int main(int argc, char **argv)
{
    using namespace Widget;

    Application::initialize(argc, argv);

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
                    button(Button::OUTLINE, "License",  [] {
                        show_license();
                    }),
                    spacer(),
                    button(Button::FILLED, "Ok", [] {
                        Application::exit(PROCESS_SUCCESS);
                    })
                })
            )
        })
    );

    // clang-format on

    win->show();

    return Application::run();
}

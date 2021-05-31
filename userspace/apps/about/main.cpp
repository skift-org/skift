#include <libsystem/BuildInfo.h>
#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Views.h>

using namespace Widget;
using namespace Graphic;

namespace About
{

static auto logo()
{
    auto logo_path = theme_is_dark()
                         ? "/Applications/about/logo-white.png"
                         : "/Applications/about/logo-black.png";

    auto logo_image = Bitmap::load_from_or_placeholder(logo_path);

    return spacing({32, 64}, image(logo_image, BitmapScaling::CENTER));
}

static auto license()
{
    return outline_button("License", [] {
        auto editor = texteditor(TextModel::open("/Files/license.md"));
        editor->font(Font::get("mono").unwrap());
        editor->focus();

        auto license_window = window(
            vflow({
                titlebar(Icon::get("information"), "License"),
                fill(editor),
            }));

        license_window->show();
    });
}

struct Application : public Widget::Application
{
    OwnPtr<Window> build() override
    {
        // clang-format off

        return window(
            vflow(4, {
                titlebar(Icon::get("information"), "About"),

                spacing(6,
                    vflow(4,{
                        logo(),
                        label("The skift operating system.", Math::Anchor::CENTER),
                        label(__BUILD_VERSION__, Math::Anchor::CENTER),
                        label(__BUILD_GITREF__ "/" __BUILD_CONFIG__, Math::Anchor::CENTER),
                        label("Copyright © 2018-2021", Math::Anchor::CENTER),
                        label("N. Van Bossuyt & contributors.", Math::Anchor::CENTER),
                    })
                ),

                spacing(6,
                    hflow(4,{
                        license(),
                        spacer(),
                        filled_button("Ok", [this] {
                            exit(PROCESS_SUCCESS);
                        })
                    })
                )
            })
        );

        // clang-format on
    }
};

} // namespace About

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    About::Application app;
    return app.run();
}

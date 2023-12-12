#include <hideo-base/alert.h>
#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

namespace Hideo::Fonts {

static constexpr Str PANGRAM = "The quick brown fox jumps over the lazy dog";

Ui::Child pangrams(Strong<Media::Fontface> fontface) {
    f64 size = 12;
    Ui::Children children;

    for (isize i = 0; i < 12; i++) {
        Media::Font font{
            .fontface = fontface,
            .fontsize = size,
        };
        children.pushBack(Ui::text(Gfx::TextStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vflow(8, children) |
           Ui::spacing(8) |
           Ui::vhscroll();
}

Ui::Child app(Res<Strong<Media::Fontface>> fontface) {
    return Ui::scafold({
        .icon = Mdi::FORMAT_FONT,
        .title = "Fonts",
        .body = fontface
                    ? pangrams(fontface.unwrap())
                    : alert("Unable to load font", fontface.none().msg()),
    });
}

} // namespace Hideo::Fonts

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);
    Res<Strong<Media::Fontface>> fontface = Error::invalidInput("No font provided");

    if (args.len()) {
        auto url = try$(Url::parseUrlOrPath(args[0]));
        fontface = Media::loadFontface(url);
    }

    return Ui::runApp(ctx, Hideo::Fonts::app(fontface));
}

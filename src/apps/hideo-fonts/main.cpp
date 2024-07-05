#include <hideo-base/alert.h>
#include <hideo-base/scafold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/scroll.h>

namespace Hideo::Fonts {

static constexpr Str PANGRAM = "The quick brown fox jumps over the lazy dog";

Ui::Child pangrams(Strong<Text::Fontface> fontface) {
    f64 size = 12;
    Ui::Children children;

    for (isize i = 0; i < 12; i++) {
        Text::Font font{
            .fontface = fontface,
            .fontsize = size,
        };
        children.pushBack(Ui::text(Text::ProseStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vflow(8, children) |
           Ui::spacing(8) |
           Ui::vhscroll();
}

Ui::Child app(Res<Strong<Text::Fontface>> fontface) {
    return scafold({
        .icon = Mdi::FORMAT_FONT,
        .title = "Fonts"s,
        .body = [fontface] {
            return fontface
                       ? pangrams(fontface.unwrap())
                       : alert(
                             "Unable to load font"s,
                             Io::toStr(fontface.none()).unwrap()
                         );
        },
    });
}

} // namespace Hideo::Fonts

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto &args = useArgs(ctx);
    Res<Strong<Text::Fontface>> fontface = Error::invalidInput("No font provided");

    if (args.len()) {
        auto url = co_try$(Mime::parseUrlOrPath(args[0]));
        fontface = Text::loadFontface(url);
    }

    co_return Ui::runApp(ctx, Hideo::Fonts::app(fontface));
}

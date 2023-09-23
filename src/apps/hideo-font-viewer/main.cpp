#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

namespace FontViewer {

static constexpr Str PANGRAM = "The quick brown fox jumps over the lazy dog";

Ui::Child pangrams(Strong<Media::Fontface> fontface) {
    f64 size = 12;
    Ui::Children children;

    for (isize i = 0; i < 12; i++) {
        Media::Font font{
            .fontface = fontface,
            .fontsize = size,
        };
        children.pushBack(Ui::text(Ui::TextStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vflow(8, children) |
           Ui::spacing(8) |
           Ui::vhscroll();
}

Ui::Child app(Strong<Media::Fontface> fontface) {
    return Ui::scafold({
        .icon = Mdi::FORMAT_FONT,
        .title = "Font Viewer",
        .body = pangrams(fontface),
    });
}

} // namespace FontViewer

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);
    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto fontface = try$(args.len()
                             ? Media::loadFontface(url)
                             : Ok(Media::Fontface::fallback()));

    return Ui::runApp(ctx, FontViewer::app(fontface));
}

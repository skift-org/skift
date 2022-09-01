#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/flow.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

static constexpr Str PANGRAM = "The quick brown fox jumps over the lazy dog";

Ui::Child pangrams(Strong<Media::Fontface> fontface) {
    double size = 12;
    Ui::Children children;

    for (int i = 0; i < 12; i++) {
        Media::Font font{size, fontface};
        children.pushBack(Ui::text(font, PANGRAM));
        size *= 1.2;
    }

    return Ui::scroll(Ui::spacing(8, Ui::vflow(8, children)));
}

void nop(Ui::Node &) {}

CliResult entryPoint(CliArgs args) {
    auto fontface = try$(args.len()
                             ? Media::loadFontface(args[0])
                             : Media::Fontface::fallback());

    auto titlebar = Ui::titlebar(
        Media::Icons::FORMAT_FONT,
        "Font Viewer");

    auto toolbar = Ui::toolbar(
        Ui::button(nop, Media::Icons::FOLDER_OPEN),
        Ui::grow(),
        Ui::button(nop, Ui::Button::PRIMARY, "INSTALL"));

    auto layout = Ui::dialogLayer(Ui::minSize(
        {700, 500},
        Ui::vflow(
            titlebar,
            toolbar,
            Ui::grow(pangrams(fontface)))));

    return Ui::runApp(args, layout);
}

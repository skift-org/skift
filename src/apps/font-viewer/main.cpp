#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>
#include <karm-ui/scafold.h>

Ui::Child pangrams(Strong<Media::Fontface> fontface) {
    double size = 12;
    Ui::Children children;

    for (int i = 0; i < 12; i++) {
        children.pushBack(Ui::text(Media::Font{size, fontface}, "The quick brown fox jumps over the lazy dog."));
        size *= 1.2;
    }

    return Ui::vflow(
        8,
        children);
}

void nop() {}

CliResult entryPoint(CliArgs args) {
    auto fontface = try$(Media::loadFontface(args[0]));

    Ui::App app([&]() {
        auto titlebar = Ui::titlebar(Media::Icons::FORMAT_FONT, String{"Font Viewer"});
        auto toolbar = Ui::toolbar(
            Ui::button(nop, Media::Icons::FOLDER_OPEN),
            Ui::spacer(),
            Ui::button(nop, Ui::Button::PRIMARY, Media::Icons::PLUS, "INSTALL FONT"));

        auto content = Ui::spacing(
            8,
            pangrams(fontface));

        return Ui::vflow(
            titlebar,
            toolbar,
            content);
    });

    return app.run(args);
}

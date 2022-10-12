#include <file-manager/widgets.h>
#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

static constexpr Str PANGRAM = "The quick brown fox jumps over the lazy dog";

Ui::Child pangrams(Strong<Media::Fontface> fontface) {
    double size = 12;
    Ui::Children children;

    for (int i = 0; i < 12; i++) {
        Media::Font font{size, fontface};
        children.pushBack(Ui::text(Ui::TextStyle{font}, PANGRAM));
        size *= 1.2;
    }

    return Ui::vhscroll(Ui::spacing(8, Ui::vflow(8, children)));
}

CliResult entryPoint(CliArgs args) {
    auto fontface = try$(args.len()
                             ? Media::loadFontface(args[0])
                             : Media::Fontface::fallback());

    auto titlebar = Ui::titlebar(
        Media::Icons::FORMAT_FONT,
        "Font Viewer");

    auto toolbar = Ui::toolbar(
        Ui::button(
            [](auto &n) {
                Ui::showDialog(n, FileManager::openFileDialog());
            },
            Ui::ButtonStyle::subtle(),
            Media::Icons::FOLDER_OPEN),

        Ui::grow(), Ui::state(false, [](auto state) {
            Func<void(Ui::Node &)> uninstallFont = [state](auto &n) mutable {
                state.update(false);
                Ui::showMsgDialog(n, "Font uninstalled", "The font was uninstalled successfully.");
            };

            Func<void(Ui::Node &)> installFont = [state](auto &n) mutable {
                state.update(true);
                Ui::showMsgDialog(n, "Font installed", "The font was installed successfully.");
            };

            if (state.value()) {
                return Ui::button(std::move(uninstallFont), Ui::ButtonStyle::destructive(), "UNINSTALL");
            } else {
                return Ui::button(std::move(installFont), Ui::ButtonStyle::primary(), "INSTALL");
            }
        }));

    auto layout = Ui::dialogLayer(Ui::minSize(
        {700, 500},
        Ui::vflow(
            titlebar,
            toolbar,
            Ui::grow(pangrams(fontface)))));

    return Ui::runApp(args, layout);
}

#include <karm-main/main.h>
#include <karm-sys/dir.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/flow.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

static void nop(Ui::Node &) {}

Ui::Child directorEntry(Sys::DirEntry const &entry) {
    return Ui::button(
        nop,
        Ui::Button::SUBTLE,
        entry.isDir ? Media::Icons::FOLDER : Media::Icons::FILE,
        entry.name);
}

Ui::Child directoryListing(Sys::Dir const &dir) {
    Ui::Children children;
    for (auto const &entry : dir.entries()) {
        children.pushBack(directorEntry(entry));
    }
    return Ui::spacing(8, Ui::vflow(children));
}

Ui::Child breadcrumbItem(Str text) {
    return Ui::button(
        nop,
        Ui::Button::SUBTLE,
        Ui::spacing(
            4,
            Ui::hflow(
                4,
                Layout::Align::CENTER,
                Ui::icon(Media::Icons::CHEVRON_RIGHT),
                Ui::text(text))));
}

Ui::Child breadcrumb() {
    return Ui::box(
        {
            .borderRadius = 4,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
        },
        Ui::hflow(
            Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::LAPTOP),
            breadcrumbItem("home"),
            breadcrumbItem("smnx"),
            breadcrumbItem("projects"),
            breadcrumbItem("skift"),
            Ui::grow(),
            Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::BOOKMARK)));
}

CliResult entryPoint(CliArgs args) {
    auto dir = try$(Sys::Dir::open("./"));

    auto titlebar = Ui::titlebar(Media::Icons::FOLDER, "File Manager");

    auto toolbar = Ui::toolbar(
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_LEFT),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_RIGHT),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_UP),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::HOME),
        Ui::grow(breadcrumb()),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::REFRESH),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::CONSOLE));

    auto listing = directoryListing(dir);

    auto layout =
        Ui::dialogLayer(
            Ui::minSize(
                {700, 500},
                Ui::vflow(
                    titlebar,
                    toolbar,
                    Ui::separator(),
                    Ui::grow(
                        Ui::scroll(
                            listing)))));

    return Ui::runApp(args, layout);
}

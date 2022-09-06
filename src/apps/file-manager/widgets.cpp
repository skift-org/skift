#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

#include "widgets.h"

namespace FileManager {

static void nop(Ui::Node &) {}

/* --- Common Widgets ------------------------------------------------------- */

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
    return Ui::grow(Ui::scroll(Ui::spacing(8, Ui::vflow(children))));
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

Ui::Child toolbar() {
    return Ui::toolbar(
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_LEFT),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_RIGHT),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::ARROW_UP),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::HOME),
        Ui::grow(FileManager::breadcrumb()),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::REFRESH),
        Ui::button(nop, Ui::Button::SUBTLE, Media::Icons::CONSOLE));
}

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog() {
    Sys::Dir dir = Sys::Dir::open("./").take();

    auto titleLbl = Ui::text(16, "Open File");
    auto msgLbl = Ui::text("Select a file to open.");

    auto openBtn = Ui::button(
        [](auto &n) {
            Ui::closeDialog(n);
        },
        Ui::Button::PRIMARY, "OPEN");

    auto cancelBtn = Ui::button(
        [](auto &n) {
            Ui::closeDialog(n);
        },
        Ui::Button::SUBTLE, "CANCEL");

    return Ui::dialogScafold(
        Layout::Align::FILL,
        Ui::vflow(
            Ui::spacing(16, Ui::vflow(8, titleLbl, msgLbl)),
            toolbar(),
            Ui::separator(),
            directoryListing(dir),
            Ui::separator()),
        {Ui::grow(), cancelBtn, openBtn});
}

/*
Ui::Child saveFileDialog() {
}

Ui::Child directoryDialog() {
}
*/

} // namespace FileManager

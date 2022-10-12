#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "model.h"
#include "widgets.h"

namespace FileManager {

/* --- Common Widgets ------------------------------------------------------- */

Ui::Child directorEntry(Sys::DirEntry const &entry) {
    return Ui::button(
        Model::bind<GoTo>(entry.name),
        Ui::ButtonStyle::subtle(),
        entry.isDir ? Media::Icons::FOLDER : Media::Icons::FILE,
        entry.name);
}

Ui::Child directoryListing(Sys::Dir const &dir) {
    Ui::Children children;
    for (auto const &entry : dir.entries()) {
        children.pushBack(directorEntry(entry));
    }
    return Ui::grow(Ui::vscroll(Ui::spacing(8, Ui::align(Layout::Align::TOP | Layout::Align::HFILL, Ui::vflow(children)))));
}

Ui::Child breadcrumbItem(Str text, int index) {
    return Ui::button(
        Model::bind<GoParent>(index),
        Ui::ButtonStyle::subtle(),
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
            Ui::button(Model::bind<GoTo>("/"), Ui::ButtonStyle::subtle(), Media::Icons::LAPTOP),
            breadcrumbItem("home", 3),
            breadcrumbItem("smnx", 2),
            breadcrumbItem("projects", 1),
            breadcrumbItem("skift", 0),
            Ui::grow(),
            Ui::button(Model::bind<AddBookmark>(), Ui::ButtonStyle::subtle(), Media::Icons::BOOKMARK)));
}

Ui::Child toolbar(State state) {

    return Ui::toolbar(
        Ui::button(Model::bindIf<GoBack>(state.canGoBack()), Ui::ButtonStyle::subtle(), Media::Icons::ARROW_LEFT),
        Ui::button(Model::bindIf<GoForward>(state.canGoForward()), Ui::ButtonStyle::subtle(), Media::Icons::ARROW_RIGHT),
        Ui::button(Model::bindIf<GoParent>(state.canGoParent(), 1), Ui::ButtonStyle::subtle(), Media::Icons::ARROW_UP),
        Ui::button(Model::bind<GoTo>("/home"), Ui::ButtonStyle::subtle(), Media::Icons::HOME),
        Ui::grow(FileManager::breadcrumb()),
        Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::subtle(), Media::Icons::REFRESH));
}

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog() {
    return Ui::reducer<FileManager::Model>({"/"}, FileManager::reduce, [](auto d) {
        Sys::Path path = d.currentPath();

        Sys::Dir dir = Sys::Dir::open(path).take();

        auto titleLbl = Ui::text(Ui::TextStyle::title1(), "Open File");
        auto msgLbl = Ui::text(Ui::TextStyle::subtitle1(), "Select a file to open.");
        auto titleBar = Ui::spacing(
            16,
            Ui::vflow(
                8,
                titleLbl,
                msgLbl));

        auto openBtn = Ui::button(
            Ui::OnPress{Ui::closeDialog},
            Ui::ButtonStyle::primary(), "OPEN");

        auto cancelBtn = Ui::button(
            Ui::closeDialog,
            Ui::ButtonStyle::subtle(), "CANCEL");

        auto controls = Ui::spacing(
            16,
            Ui::hflow(
                8,
                Ui::grow(),
                cancelBtn,
                openBtn));

        return Ui::dialogScafold(
            Layout::Align::FILL,
            Ui::vflow(
                Ui::grow(
                    Ui::vflow(
                        titleBar,
                        toolbar(d),
                        directoryListing(dir),
                        Ui::separator())),
                controls));
    });
}

/*
Ui::Child saveFileDialog() {
}

Ui::Child directoryDialog() {
}
*/

} // namespace FileManager

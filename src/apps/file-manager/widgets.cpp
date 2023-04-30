#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

#include "model.h"
#include "widgets.h"

namespace FileManager {

/* --- Common Widgets ------------------------------------------------------- */

Ui::Child directorEntry(Sys::DirEntry const &entry) {
    return Ui::button(
        Model::bind<Navigate>(entry.name),
        Ui::ButtonStyle::subtle(),
        entry.isDir ? Mdi::FOLDER : Mdi::FILE,
        entry.name);
}

Ui::Child directoryListing(Sys::Dir const &dir) {
    Ui::Children children;
    for (auto const &entry : dir.entries()) {
        children.pushBack(directorEntry(entry));
    }

    return Ui::vflow(children) |
           Ui::align(Layout::Align::TOP | Layout::Align::HFILL) |
           Ui::spacing(8) |
           Ui::vscroll() |
           Ui::grow();
}

Ui::Child breadcrumbItem(Str text, isize index) {
    return Ui::button(
        Model::bind<GoParent>(index),
        Ui::ButtonStyle::subtle(),
        Ui::spacing(
            4,
            Ui::hflow(
                4,
                Layout::Align::CENTER,
                Ui::icon(Mdi::CHEVRON_RIGHT),
                Ui::text(text))));
}

Ui::Child breadcrumb(State const &state) {
    return Ui::box(
        {
            .borderRadius = 4,
            .borderWidth = 1,
            .backgroundPaint = Ui::GRAY800,
        },
        Ui::hflow(
            Ui::button(Model::bind<GoRoot>(), Ui::ButtonStyle::subtle(), Mdi::LAPTOP),

            Ui::hflow(state
                          .currentUrl()
                          .iter()
                          .mapi(breadcrumbItem)
                          .collect<Ui::Children>()),

            Ui::grow(NONE),
            Ui::button(Model::bind<AddBookmark>(), Ui::ButtonStyle::subtle(), Mdi::BOOKMARK)));
}

Ui::Child toolbar(State const &state) {
    return Ui::toolbar(
        Ui::button(Model::bindIf<GoBack>(state.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
        Ui::button(Model::bindIf<GoForward>(state.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
        Ui::button(Model::bindIf<GoParent>(state.canGoParent(), 1), Ui::ButtonStyle::subtle(), Mdi::ARROW_UP),
        Ui::button(Model::bind<GoHome>(), Ui::ButtonStyle::subtle(), Mdi::HOME),
        Ui::grow(FileManager::breadcrumb(state)),
        Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::subtle(), Mdi::REFRESH));
}

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog() {
    return Ui::reducer<FileManager::Model>(
        {"file:/"_url},
        [](auto d) {
            Sys::Dir dir = Sys::Dir::open(d.currentUrl()).take();

            auto titleLbl = Ui::text(
                Ui::TextStyle::titleLarge(), "Open File");

            auto msgLbl = Ui::text(
                Ui::TextStyle::titleMedium(), "Select a file to open.");

            auto titleBar =
                Ui::vflow(8, titleLbl, msgLbl) |
                Ui::spacing(16);

            auto openBtn = Ui::button(
                Ui::closeDialog,
                Ui::ButtonStyle::primary(), "OPEN");

            auto cancelBtn = Ui::button(
                Ui::closeDialog,
                Ui::ButtonStyle::subtle(), "CANCEL");

            auto controls = Ui::spacing(
                16,
                Ui::hflow(
                    8,
                    Ui::grow(NONE),
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

} // namespace FileManager

#include <hideo-base/alert.h>
#include <hideo-base/dialogs.h>
#include <hideo-base/scafold.h>
#include <karm-mime/mime.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/scroll.h>

#include "model.h"
#include "widgets.h"

namespace Hideo::Files {

/* --- Common Widgets ------------------------------------------------------- */

Ui::Child alert(State const &state, String title, String body) {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               Hideo::alert(title, body),
               Ui::hflow(
                   16,
                   Ui::button(Model::bindIf<GoBack>(state.canGoBack()), "Go Back"),
                   Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::primary(), "Retry")
               )
           ) |
           Ui::center();
}

Ui::ButtonStyle itemStyle(bool odd) {
    auto background = odd ? Ui::GRAY50.withOpacity(0.04) : Gfx::ALPHA;
    return {
        .idleStyle = {
            .backgroundPaint = background,
            .foregroundPaint = Ui::GRAY300,
        },
        .hoverStyle = {
            .borderWidth = 1,
            .backgroundPaint = Ui::ACCENT900,
        },
        .pressStyle = {
            .borderWidth = 1,
            .borderPaint = Ui::ACCENT900,
            .backgroundPaint = Ui::ACCENT950,
        },
    };
}

Ui::Child directorEntry(Sys::DirEntry const &entry, bool odd) {
    return Ui::button(
        Model::bind<Navigate>(entry.name),
        itemStyle(odd),
        entry.isDir ? Mdi::FOLDER : Mime::iconFor(tryOr(Mime::sniffSuffix(Mime::suffixOf(entry.name)), "file"s)),
        entry.name
    );
}

Ui::Child directoryListing(State const &, Sys::Dir const &dir) {
    if (dir.entries().len() == 0)
        return Ui::bodyMedium(Ui::GRAY500, "This directory is empty.") | Ui::center();

    Ui::Children children;
    bool odd = true;
    for (auto const &entry : dir.entries()) {
        children.pushBack(directorEntry(entry, odd));
        odd = !odd;
    }

    return Ui::vflow(children) |
           Ui::align(Layout::Align::TOP | Layout::Align::HFILL) |
           Ui::vscroll() |
           Ui::grow();
}

Ui::Child breadcrumbItem(Str text, isize index) {
    return Ui::hflow(
        0,
        Layout::Align::CENTER,
        Ui::icon(Mdi::CHEVRON_RIGHT),
        Ui::button(Model::bind<GoParent>(index), Ui::ButtonStyle::text().withPadding({2, 0}), Ui::text(text))
    );
}

Mdi::Icon iconForLocation(Str loc) {
    if (eqCi(loc, "home"s))
        return Mdi::HOME;

    if (eqCi(loc, "documents"s))
        return Mdi::FILE_DOCUMENT;

    if (eqCi(loc, "pictures"s))
        return Mdi::IMAGE;

    if (eqCi(loc, "music"s))
        return Mdi::MUSIC;

    if (eqCi(loc, "videos"s))
        return Mdi::FILM;

    if (eqCi(loc, "downloads"s))
        return Mdi::DOWNLOAD;

    return Mdi::FOLDER;
}

Mdi::Icon iconForUrl(Mime::Url const &url) {
    if (url.scheme == "location")
        return iconForLocation(url.host);

    if (url.scheme == "device")
        return Mdi::HARDDISK;

    return Mdi::LAPTOP;
}

String textForUrl(Mime::Url const &url) {
    if (url.scheme == "location")
        return Io::toTitleCase(url.host).unwrap();

    if (url.scheme == "device")
        return url.host;

    return "This Device"s;
}

Ui::Child breadcrumbRoot(Mime::Url const &url) {
    return Ui::button(
        Model::bind<GoRoot>(),
        Ui::ButtonStyle::text(),
        Ui::hflow(
            8,
            Layout::Align::CENTER,
            Ui::icon(iconForUrl(url)),
            Ui::text(textForUrl(url))
        )
    );
}

Ui::Child breadcrumb(State const &state) {
    return Ui::box(
        {
            .borderRadius = 4,
            .borderWidth = 1,
            .backgroundPaint = Ui::GRAY800,
        },
        Ui::hflow(
            Ui::empty(12),
            breadcrumbRoot(state.currentUrl()),

            Ui::hflow(state
                          .currentUrl()
                          .iter()
                          .mapi([&](auto const &text, usize i) {
                              return breadcrumbItem(text, state.currentUrl().len() - i - 1);
                          })
                          .collect<Ui::Children>()) |
                Ui::hscroll() |
                Ui::grow(),
            Ui::button(Model::bind<AddBookmark>(), Ui::ButtonStyle::subtle(), Mdi::BOOKMARK)
        )
    );
}

Ui::Child goBackTool(State const &state) {
    return Ui::button(
        Model::bindIf<GoBack>(state.canGoBack()),
        Ui::ButtonStyle::subtle(),
        Mdi::ARROW_LEFT
    );
}

Ui::Child goForwardTool(State const &state) {
    return Ui::button(
        Model::bindIf<GoForward>(state.canGoForward()),
        Ui::ButtonStyle::subtle(),
        Mdi::ARROW_RIGHT
    );
}

Ui::Child goParentTool(State const &state) {
    return Ui::button(
        Model::bindIf<GoParent>(state.canGoParent(), 1),
        Ui::ButtonStyle::subtle(),
        Mdi::ARROW_UP
    );
}

Ui::Child refreshTool() {
    return Ui::button(
        Model::bind<Refresh>(),
        Ui::ButtonStyle::subtle(),
        Mdi::REFRESH
    );
}

Ui::Child toolbar(State const &state) {
    return Hideo::toolbar(
        Ui::button(Model::bindIf<GoBack>(state.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
        Ui::button(Model::bindIf<GoForward>(state.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
        Ui::button(Model::bindIf<GoParent>(state.canGoParent(), 1), Ui::ButtonStyle::subtle(), Mdi::ARROW_UP),
        Ui::grow(breadcrumb(state)),
        Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::subtle(), Mdi::REFRESH)
    );
}

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog() {
    return Ui::reducer<Model>(
        {"file:/"_url},
        [](auto d) {
            auto maybeDir = Sys::Dir::open(d.currentUrl());

            auto titleLbl = Ui::titleLarge("Open File");

            auto msgLbl = Ui::titleMedium("Select a file to open.");

            auto titleBar =
                Ui::vflow(8, titleLbl, msgLbl) |
                Ui::spacing(16);

            auto openBtn = Ui::button(
                Ui::closeDialog,
                Ui::ButtonStyle::primary(), "Open"
            );

            auto cancelBtn = Ui::button(
                Ui::closeDialog,
                Ui::ButtonStyle::subtle(), "Cancel"
            );

            auto controls = Ui::spacing(
                16,
                Ui::hflow(
                    8,
                    Ui::grow(NONE),
                    cancelBtn,
                    openBtn
                )
            );

            return dialogScafold(
                Layout::Align::FILL,
                Ui::vflow(
                    Ui::grow(
                        Ui::vflow(
                            titleBar,
                            toolbar(d),
                            maybeDir
                                ? directoryListing(d, maybeDir.unwrap())
                                : alert(
                                      d,
                                      "Can't access location"s,
                                      Io::toStr(maybeDir.none()).unwrap()
                                  ),
                            Ui::separator()
                        )
                    ),
                    controls
                )
            );
        }
    );
}

} // namespace Hideo::Files

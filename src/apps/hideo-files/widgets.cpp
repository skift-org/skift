#include <karm-kira/context-menu.h>
#include <karm-kira/dialog.h>
#include <karm-kira/error-page.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/toolbar.h>
#include <karm-mime/mime.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <mdi/alert-decagram.h>
#include <mdi/alert-outline.h>
#include <mdi/arrow-left.h>
#include <mdi/arrow-right.h>
#include <mdi/arrow-up.h>
#include <mdi/bookmark.h>
#include <mdi/chevron-right.h>
#include <mdi/content-copy.h>
#include <mdi/content-cut.h>
#include <mdi/content-paste.h>
#include <mdi/delete-outline.h>
#include <mdi/download.h>
#include <mdi/file-document.h>
#include <mdi/film.h>
#include <mdi/folder.h>
#include <mdi/form-textbox.h>
#include <mdi/harddisk.h>
#include <mdi/home.h>
#include <mdi/image.h>
#include <mdi/information-outline.h>
#include <mdi/laptop.h>
#include <mdi/magnify.h>
#include <mdi/music.h>
#include <mdi/pencil.h>
#include <mdi/refresh.h>
#include <mdi/share.h>

#include "model.h"
#include "widgets.h"

namespace Hideo::Files {

// MARK: Common Widgets --------------------------------------------------------

Ui::Child alert(State const &state, String title, String body) {
    return Kr::errorPageContent({
        Kr::errorPageTitle(Mdi::ALERT_DECAGRAM, title),
        Kr::errorPageBody(body),
        Kr::errorPageFooter({
            Ui::button(Model::bindIf<GoBack>(state.canGoBack()), "Go Back"),
            Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::primary(), "Retry"),
        }),
    });
}

Ui::ButtonStyle itemStyle(bool odd) {
    auto background = odd ? Ui::GRAY50.withOpacity(0.04) : Gfx::ALPHA;
    return {
        .idleStyle = {
            .backgroundFill = background,
            .foregroundFill = Ui::GRAY300,
        },
        .hoverStyle = {
            .borderWidth = 1,
            .backgroundFill = Ui::ACCENT900,
        },
        .pressStyle = {
            .borderWidth = 1,
            .borderFill = Ui::ACCENT900,
            .backgroundFill = Ui::ACCENT950,
        },
    };
}

Ui::Child directoryContextMenu() {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Ui::NOP, Mdi::CONTENT_COPY),
            Kr::contextMenuIcon(Ui::NOP, Mdi::CONTENT_CUT),
            // Kr::contextMenuIcon(Ui::NOP, Mdi::CONTENT_PASTE),
            Kr::contextMenuIcon(Ui::NOP, Mdi::FORM_TEXTBOX),
            Ui::grow(NONE),
            Ui::separator(),
            Kr::contextMenuIcon(Ui::NOP, Mdi::DELETE_OUTLINE),
        }),
        Ui::separator(),
        Kr::contextMenuItem(Ui::NOP, Mdi::MAGNIFY, "Preview"),
        Kr::contextMenuItem(Ui::NOP, Mdi::PENCIL, "Modify"),
        Kr::contextMenuItem(Ui::NOP, Mdi::SHARE, "Interact…"),
        Ui::separator(),
        Kr::contextMenuItem(Ui::NOP, Mdi::INFORMATION_OUTLINE, "Properties"),
    });
}

Ui::Child directorEntry(Sys::DirEntry const &entry, bool odd) {
    return Ui::button(
               Model::bind<Navigate>(entry.name),
               itemStyle(odd),
               entry.type == Sys::Type::DIR
                   ? Mdi::FOLDER
                   : Mime::iconFor(Mime::sniffSuffix(Mime::suffixOf(entry.name)).unwrapOr("file"s)),
               entry.name
           ) |
           Kr::contextMenu(slot$(directoryContextMenu()));
}

Ui::Child directoryListing(State const &s, Sys::Dir const &dir) {
    if (dir.entries().len() == 0)
        return Ui::bodyMedium(Ui::GRAY500, "This directory is empty.") | Ui::center();

    Ui::Children children;
    bool odd = true;
    for (auto const &entry : dir.entries()) {
        children.pushBack(directorEntry(entry, odd));
        odd = !odd;
    }

    return Ui::vflow(children) |
           Ui::align(Math::Align::TOP | Math::Align::HFILL) |
           Ui::vscroll() | Ui::key(s.currentIndex);
}

Ui::Child breadcrumbItem(Str text, isize index) {
    return Ui::hflow(
        0,
        Math::Align::CENTER,
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
            Math::Align::CENTER,
            Ui::icon(iconForUrl(url)),
            Ui::text(textForUrl(url))
        )
    );
}

Ui::Child breadcrumb(State const &state) {
    return Ui::box(
        {
            .borderRadii = 4,
            .borderWidth = 1,
            .backgroundFill = Ui::GRAY800,
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
    return Kr::toolbar({
        Ui::button(Model::bindIf<GoBack>(state.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
        Ui::button(Model::bindIf<GoForward>(state.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
        Ui::button(Model::bindIf<GoParent>(state.canGoParent(), 1), Ui::ButtonStyle::subtle(), Mdi::ARROW_UP),
        Ui::grow(breadcrumb(state)),
        Ui::button(Model::bind<Refresh>(), Ui::ButtonStyle::subtle(), Mdi::REFRESH),
    });
}

// MARK:  Dialogs  -------------------------------------------------------------

Ui::Child openFileDialog() {
    return Ui::reducer<Model>(
        {"file:/"_url},
        [](auto const &d) {
            auto maybeDir = Sys::Dir::open(d.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Open file…"s),
                toolbar(d),
                (maybeDir
                     ? directoryListing(d, maybeDir.unwrap())
                     : alert(
                           d,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none()).unwrap()
                       )
                ) | Ui::pinSize({400, 260}),
                Ui::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(Ui::NOP, "Open"s),
                }),
            });
        }
    );
}

} // namespace Hideo::Files

module;

#include <karm-app/inputs.h>
#include <karm-gfx/icon.h>
#include <karm-math/align.h>
#include <karm-mime/mime.h>
#include <karm-sys/dir.h>

export module Hideo.Files:widgets;

import Karm.Ui;
import Karm.Kira;
import Mdi;
import :model;

namespace Hideo::Files {

struct Mime2Icon {
    Str type;
    Str subtype;
    Gfx::Icon icon;
};

static Array MIME2ICON = {
    Mime2Icon{"text", "", Mdi::FILE_DOCUMENT},
    Mime2Icon{"text", "html", Mdi::LANGUAGE_HTML5},
    Mime2Icon{"text", "css", Mdi::LANGUAGE_CSS3},
    Mime2Icon{"text", "javascript", Mdi::LANGUAGE_JAVASCRIPT},
    Mime2Icon{"text", "plain", Mdi::FILE_DOCUMENT},

    Mime2Icon{"image", "", Mdi::IMAGE},
    Mime2Icon{"image", "jpeg", Mdi::FILE_JPG_BOX},
    Mime2Icon{"image", "png", Mdi::FILE_PNG_BOX},

    Mime2Icon{"video", "", Mdi::FILMSTRIP},

    Mime2Icon{"font", "", Mdi::FORMAT_FONT},

    Mime2Icon{"application", "pdf", Mdi::FILE_PDF_BOX},
    Mime2Icon{"application", "json", Mdi::CODE_JSON},
    Mime2Icon{"application", "zip", Mdi::ZIP_BOX},
    Mime2Icon{"application", "tar", Mdi::ZIP_BOX},
    Mime2Icon{"application", "gz", Mdi::ZIP_BOX},
    Mime2Icon{"application", "bz2", Mdi::ZIP_BOX},
    Mime2Icon{"application", "7z-compressed", Mdi::ZIP_BOX},
    Mime2Icon{"application", "rar", Mdi::ZIP_BOX},
    Mime2Icon{"application", "x-xz", Mdi::ZIP_BOX},
    Mime2Icon{"application", "x-msdownload", Mdi::COG_BOX},
};

Gfx::Icon iconFor(Mime::Mime const& mime) {
    Gfx::Icon icon = Mdi::FILE;

    for (auto const& m : MIME2ICON) {
        if (m.type == mime.type() and m.subtype == mime.subtype())
            return m.icon;

        if (m.type == mime.type() and m.subtype == "")
            icon = m.icon;
    }

    return icon;
}

// MARK: Common Widgets --------------------------------------------------------

export Ui::Child alert(State const& state, String title, String body) {
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
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::CONTENT_COPY),
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::CONTENT_CUT),
            // Kr::contextMenuIcon(Ui::SINK, Mdi::CONTENT_PASTE),
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::FORM_TEXTBOX),
            Ui::grow(NONE),
            Kr::separator(),
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::DELETE_OUTLINE),
        }),
        Kr::separator(),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::MAGNIFY, "Preview"),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::PENCIL, "Modify"),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::SHARE, "Interact…"),
        Kr::separator(),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::INFORMATION_OUTLINE, "Properties"),
    });
}

Ui::Child directorEntry(Sys::DirEntry const& entry, bool odd) {
    return Ui::button(
               Model::bind<Navigate>(entry.name),
               itemStyle(odd),
               entry.type == Sys::Type::DIR
                   ? Mdi::FOLDER
                   : iconFor(Mime::sniffSuffix(Mime::suffixOf(entry.name)).unwrapOr("file"s)),
               entry.name
           ) |
           Kr::contextMenu(directoryContextMenu);
}

Ui::Child directoryListing(State const& s, Sys::Dir const& dir) {
    if (dir.entries().len() == 0)
        return Ui::bodyMedium(Ui::GRAY500, "This directory is empty.") | Ui::center();

    Ui::Children children;
    bool odd = true;
    for (auto const& entry : dir.entries()) {
        if (entry.hidden() and not s.showHidden)
            continue;
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

Gfx::Icon iconForLocation(Str loc) {
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

Gfx::Icon iconForUrl(Mime::Url const& url) {
    if (url.scheme == "location")
        return iconForLocation(url.host);

    if (url.scheme == "device")
        return Mdi::HARDDISK;

    return Mdi::LAPTOP;
}

String textForUrl(Mime::Url const& url) {
    if (url.scheme == "location")
        return Io::toTitleCase(url.host).unwrap();

    if (url.scheme == "device")
        return url.host;

    return "This Device"s;
}

Ui::Child breadcrumbRoot(Mime::Url const& url) {
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

export Ui::Child refreshTool() {
    return Ui::button(
               Model::bind<Refresh>(),
               Ui::ButtonStyle::subtle(),
               Mdi::REFRESH
           ) |
           Ui::keyboardShortcut(App::Key::R, App::KeyMod::ALT);
}

export Ui::Child breadcrumb(State const& s) {
    Ui::Children items;
    items.pushBack(breadcrumbRoot(s.currentUrl()));
    s
        .currentUrl()
        .iter()
        .mapi([&](auto const& text, usize i) {
            items.pushBack(breadcrumbItem(text, s.currentUrl().len() - i - 1));
            return true;
        })
        .collect();

    return Ui::box(
               {
                   .borderRadii = 4,
                   .borderWidth = 1,
                   .backgroundFill = Ui::GRAY800,
               },
               Ui::hflow(
                   Ui::empty(12),
                   Ui::hflow(items) |
                       Ui::hscroll() |
                       Ui::grow(),
                   refreshTool()
               )
           ) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::L, App::KeyMod::CTRL);
}

export Ui::Child goBackTool(State const& s) {
    return Ui::button(
               Model::bindIf<GoBack>(s.canGoBack()),
               Ui::ButtonStyle::subtle(),
               Mdi::ARROW_LEFT
           ) |
           Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT);
}

export Ui::Child goForwardTool(State const& s) {
    return Ui::button(
               Model::bindIf<GoForward>(s.canGoForward()),
               Ui::ButtonStyle::subtle(),
               Mdi::ARROW_RIGHT
           ) |
           Ui::keyboardShortcut(App::Key::RIGHT, App::KeyMod::ALT);
}

export Ui::Child goParentTool(State const& s) {
    return Ui::button(
               Model::bindIf<GoParent>(s.canGoParent(), 1),
               Ui::ButtonStyle::subtle(),
               Mdi::ARROW_UP
           ) |
           Ui::keyboardShortcut(App::Key::UP, App::KeyMod::ALT);
}

export Ui::Child mainMenu([[maybe_unused]] State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::SINK<>,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::BOOKMARK, "Bookmarks"),
        Kr::separator(),
        Kr::contextMenuCheck(Model::bind<ToggleHidden>(), s.showHidden, "Show hidden"),
    });
}

export Ui::Child moreTool(State const& s) {
    return Ui::button(
        [&](Ui::Node& n) {
            Ui::showPopover(n, n.bound().bottomEnd(), mainMenu(s));
        },
        Ui::ButtonStyle::subtle(),
        Mdi::DOTS_HORIZONTAL
    );
}

export Ui::Child toolbar(State const& s) {
    return Kr::toolbar({
        goBackTool(s),
        goForwardTool(s),
        goParentTool(s),
        Ui::grow(breadcrumb(s)),
        moreTool(s),
    });
}

} // namespace Hideo::Files

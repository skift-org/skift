module;

#include <karm-gfx/icon.h>
#include <karm-sys/dir.h>

export module Hideo.Files:app;

import Mdi;
import Karm.Kira;
import Karm.Ui;
import :model;
import :widgets;

namespace Hideo::Files {

Ui::Child sidenavItem(State const& s, Gfx::Icon icon, String title, Ref::Url url) {
    bool selected = url.isParentOf(s.currentUrl());
    return Kr::sidenavItem(selected, Model::bind<GoTo>(url), icon, title);
}

Ui::Child sidebar(State const& s) {
    return Kr::sidenav({
        Kr::searchbar(""s) | Ui::insets({6, 0}),
        sidenavItem(s, Mdi::HOME, "Home"s, "location://home"_url),
        sidenavItem(s, Mdi::FILE_DOCUMENT, "Documents"s, "location://documents"_url),
        sidenavItem(s, Mdi::IMAGE, "Pictures"s, "location://pictures"_url),
        sidenavItem(s, Mdi::MUSIC, "Music"s, "location://music"_url),
        sidenavItem(s, Mdi::FILM, "Videos"s, "location://videos"_url),
        sidenavItem(s, Mdi::DOWNLOAD, "Downloads"s, "location://downloads"_url),
        Kr::separator(),
        sidenavItem(s, Mdi::LAPTOP, "This Device"s, "file:/"_url),
        sidenavItem(s, Mdi::USB, "USB"s, "device://usb"_url),
        sidenavItem(s, Mdi::SD, "SD Card"s, "device://sdcard"_url),
        sidenavItem(s, Mdi::DISC, "CD-ROM"s, "device://cdrom"_url),
    });
}

Ui::Child pageContent(State const& state) {
    auto url = state.currentUrl();
    auto dir = Sys::Dir::open(url);
    auto listing =
        dir
            ? directoryListing(state, dir.unwrap()) | Ui::grow()
            : alert(
                  state,
                  "Can't access this location"s,
                  Io::toStr(dir.none())
              );

    return listing | Ui::grow();
}

export Ui::Child app() {
    return Ui::reducer<Model>("location://home"_url, [](State const& s) {
        return Kr::scaffold({
            .icon = Mdi::FOLDER,
            .title = "Files"s,
            .startTools = [&] -> Ui::Children {
                return {
                    goBackTool(s),
                    goForwardTool(s),
                    goParentTool(s),
                };
            },
            .middleTools = [&] -> Ui::Children {
                return {
                    breadcrumb(s) | Ui::grow(),
                };
            },
            .endTools = [&] -> Ui::Children {
                return {
                    moreTool(s),
                };
            },
            .sidebar = [&] {
                return sidebar(s);
            },
            .body = [&] {
                return pageContent(s);
            },
        });
    });
}

} // namespace Hideo::Files

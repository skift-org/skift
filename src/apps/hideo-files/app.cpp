#include <karm-kira/scaffold.h>
#include <karm-kira/searchbar.h>
#include <karm-kira/side-nav.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <mdi/disc.h>
#include <mdi/download.h>
#include <mdi/file-document.h>
#include <mdi/film.h>
#include <mdi/folder.h>
#include <mdi/home.h>
#include <mdi/image.h>
#include <mdi/laptop.h>
#include <mdi/music.h>
#include <mdi/sd.h>
#include <mdi/usb.h>

#include "model.h"
#include "widgets.h"

namespace Hideo::Files {

Ui::Child sidenavItem(State const &s, Mdi::Icon icon, String title, Mime::Url url) {
    bool selected = url.isParentOf(s.currentUrl());
    return Kr::sidenavItem(selected, Model::bind<GoTo>(url), icon, title);
}

Ui::Child sidebar(State const &s) {
    return Kr::sidenav({
        Kr::searchbar() | Ui::insets({6, 0}),
        sidenavItem(s, Mdi::HOME, "Home"s, "location://home"_url),
        sidenavItem(s, Mdi::FILE_DOCUMENT, "Documents"s, "location://documents"_url),
        sidenavItem(s, Mdi::IMAGE, "Pictures"s, "location://pictures"_url),
        sidenavItem(s, Mdi::MUSIC, "Music"s, "location://music"_url),
        sidenavItem(s, Mdi::FILM, "Videos"s, "location://videos"_url),
        sidenavItem(s, Mdi::DOWNLOAD, "Downloads"s, "location://downloads"_url),
        Ui::separator(),
        sidenavItem(s, Mdi::LAPTOP, "This Device"s, "file:/"_url),
        sidenavItem(s, Mdi::USB, "USB"s, "device://usb"_url),
        sidenavItem(s, Mdi::SD, "SD Card"s, "device://sdcard"_url),
        sidenavItem(s, Mdi::DISC, "CD-ROM"s, "device://cdrom"_url),
    });
}

Ui::Child pageContent(State const &state) {
    auto url = state.currentUrl();
    auto dir = Sys::Dir::open(url);
    auto listing = dir
                       ? directoryListing(state, dir.unwrap()) | Ui::grow()
                       : alert(
                             state,
                             "Can't access this location"s,
                             Io::toStr(dir.none()).unwrap()
                         );

    return listing | Ui::grow();
}

Ui::Child app() {
    return Ui::reducer<Model>("location://home"_url, [](State const &s) {
        return Kr::scaffold({
            .icon = Mdi::FOLDER,
            .title = "Files"s,
            .startTools = slots$(
                goBackTool(s),
                goForwardTool(s),
                goParentTool(s)
            ),
            .midleTools = slots$(breadcrumb(s) | Ui::grow()),
            .endTools = slots$(moreTool(s)),
            .sidebar = slot$(sidebar(s)),
            .body = slot$(pageContent(s)),
        });
    });
}

} // namespace Hideo::Files

#include <hideo-base/sidenav.h>
#include <karm-main/main.h>
#include <karm-ui/anim.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>

#include "../model.h"
#include "../widgets.h"

namespace Hideo::Files {

Ui::Child sidebar(State const &) {
    Ui::Children items = {
        Hideo::sidenavTree(
            Mdi::BOOKMARK,
            "Bookmarks",
            Ui::empty()),

        Hideo::sidenavTree(
            Mdi::HOME,
            "Locations",
            Ui::vflow(
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://documents"_url), Mdi::FILE_DOCUMENT, "Documents"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://images"_url), Mdi::IMAGE, "Pictures"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://musics"_url), Mdi::MUSIC, "Music"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://videos"_url), Mdi::FILM, "Videos"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://downloads"_url), Mdi::DOWNLOAD, "Downloads"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("location://trash"_url), Mdi::TRASH_CAN, "Trash"))),

        Hideo::sidenavTree(
            Mdi::HARDDISK,
            "Devices",
            Ui::vflow(
                Hideo::sidenavItem(false, Model::bind<GoTo>("device://root"_url), Mdi::SERVER_NETWORK, "Root"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("device://usb"_url), Mdi::USB, "USB"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("device://sdcard"_url), Mdi::SD, "SD Card"),
                Hideo::sidenavItem(false, Model::bind<GoTo>("device://cdrom"_url), Mdi::DISC, "CD-ROM"))),
    };

    return Hideo::sidenav(items);
}

Ui::Child pageContent(State const &state) {
    auto url = state.currentUrl();
    auto dir = Sys::Dir::open(url);
    auto listing = dir
                       ? directoryListing(state, dir.unwrap())
                       : alert(state, "Can't access this location", url.str());

    return listing | Ui::grow();
}

Ui::Child app() {
    return Ui::reducer<Model>("file:/"_url, [](auto state) {
        return Ui::scafold({
            .icon = Mdi::FOLDER,
            .title = "Files",
            .startTools = {
                goBackTool(state),
                goForwardTool(state),
                goParentTool(state),
            },
            .midleTools = {breadcrumb(state) | Ui::grow()},
            .endTools = {
                refreshTool(),
            },
            .sidebar = sidebar(state),
            .body = pageContent(state),
        });
    });
}

} // namespace Hideo::Files

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Files::app());
}

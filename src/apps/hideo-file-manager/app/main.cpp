#include <karm-main/main.h>
#include <karm-ui/anim.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>

#include "../model.h"
#include "../widgets.h"

namespace FileManager {

Ui::Child sidebar(State const &) {
    Ui::Children items = {
        Ui::navTree(
            Mdi::BOOKMARK,
            "Bookmarks",
            Ui::empty()),

        Ui::navTree(
            Mdi::HOME,
            "Locations",
            Ui::vflow(
                Ui::navRow(false, Model::bind<GoTo>("location://documents"_url), Mdi::FILE_DOCUMENT, "Documents"),
                Ui::navRow(false, Model::bind<GoTo>("location://images"_url), Mdi::IMAGE, "Pictures"),
                Ui::navRow(false, Model::bind<GoTo>("location://musics"_url), Mdi::MUSIC, "Music"),
                Ui::navRow(false, Model::bind<GoTo>("location://videos"_url), Mdi::FILM, "Videos"),
                Ui::navRow(false, Model::bind<GoTo>("location://downloads"_url), Mdi::DOWNLOAD, "Downloads"),
                Ui::navRow(false, Model::bind<GoTo>("location://trash"_url), Mdi::TRASH_CAN, "Trash"))),

        Ui::navTree(
            Mdi::HARDDISK,
            "Devices",
            Ui::vflow(
                Ui::navRow(false, Model::bind<GoTo>("device://root"_url), Mdi::SERVER_NETWORK, "Root"),
                Ui::navRow(false, Model::bind<GoTo>("device://usb"_url), Mdi::USB, "USB"),
                Ui::navRow(false, Model::bind<GoTo>("device://sdcard"_url), Mdi::SD, "SD Card"),
                Ui::navRow(false, Model::bind<GoTo>("device://cdrom"_url), Mdi::DISC, "CD-ROM"))),
    };

    return Ui::navList(items);
}

Ui::Child pageContent(State const &state) {
    auto url = state.currentUrl();
    auto dir = Sys::Dir::open(url);
    auto listing = dir
                       ? FileManager::directoryListing(state, dir.unwrap())
                       : FileManager::alert(state, "Can't access this location", url.str().unwrap());

    return listing | Ui::grow();
}

Ui::Child app() {
    return Ui::reducer<FileManager::Model>("file:/"_url, [](auto state) {
        return Ui::scafold({
            .icon = Mdi::FOLDER,
            .title = "File Manager",
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

} // namespace FileManager

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, FileManager::app());
}

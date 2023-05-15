#include <file-manager/model.h>
#include <file-manager/widgets.h>
#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>

namespace FileManager {

Ui::Child app() {
    return Ui::reducer<FileManager::Model>("file:/"_url, [](auto d) {
        auto url = d.currentUrl();
        auto dir = Sys::Dir::open(url);
        auto titlebar = Ui::titlebar(Mdi::FOLDER, "File Manager");
        auto listing = dir
                           ? FileManager::directoryListing(d, dir.unwrap())
                           : FileManager::alert(d, "Can't access this location", url.str().unwrap());

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

        return Ui::dialogLayer(
            Ui::pinSize(
                {800, 600},
                Ui::vflow(
                    titlebar,
                    FileManager::toolbar(d),
                    Ui::hflow(
                        Ui::navList(items),
                        Ui::separator(),
                        listing | Ui::grow()) |
                        Ui::grow())));
    });
}

} // namespace FileManager

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, FileManager::app());
}

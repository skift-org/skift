#include <file-manager/model.h>
#include <file-manager/widgets.h>
#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>

namespace FileManager {

Ui::Child app() {
    return Ui::reducer<FileManager::Model>({"/"}, [](auto d) {
        Sys::Path path = d.currentPath();
        auto dir = Sys::Dir::open(path).unwrap();
        auto titlebar = Ui::titlebar(Mdi::FOLDER, "File Manager");
        auto listing = FileManager::directoryListing(dir);

        return Ui::dialogLayer(
            Ui::minSize(
                {700, 500},
                Ui::vflow(
                    titlebar,
                    FileManager::toolbar(d),
                    listing)));
    });
}

} // namespace FileManager

Res<> entryPoint(CliArgs args) {
    return Ui::runApp(args, FileManager::app());
}

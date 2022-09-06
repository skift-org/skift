#include <file-manager/widgets.h>
#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>

CliResult entryPoint(CliArgs args) {
    auto dir = try$(Sys::Dir::open("./"));

    auto titlebar = Ui::titlebar(Media::Icons::FOLDER, "File Manager");

    auto listing = FileManager::directoryListing(dir);

    auto layout =
        Ui::dialogLayer(
            Ui::minSize(
                {700, 500},
                Ui::vflow(
                    titlebar,
                    FileManager::toolbar(),
                    Ui::separator(),
                    listing)));

    return Ui::runApp(args, layout);
}

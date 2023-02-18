#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

Res<> entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(Media::Icons::IMAGE, "Image Viewer");

    auto content = Ui::image(try$(Media::loadImage(args[0])));

    auto layout = Ui::maxSize(
        {700, 500},
        Ui::vflow(
            titlebar,
            Ui::grow(
                Ui::fit(
                    content))));

    return Ui::runApp(args, Ui::dialogLayer(layout));
}

#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>

#include "app.h"

namespace ImageViewer {

Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const &state) {
            auto titlebar = Ui::titlebar(Mdi::IMAGE, "Image Viewer");

            auto content = state.isEditor
                               ? editor(state)
                               : viewer(state);

            return Ui::vflow(titlebar, content | Ui::grow()) |
                   Ui::maxSize({700, 500}) |
                   Ui::dialogLayer();
        });
}

} // namespace ImageViewer

Res<> entryPoint(CliArgs args) {
    auto image = try$(Media::loadImage(args[0]));
    return Ui::runApp(args, ImageViewer::app(image));
}

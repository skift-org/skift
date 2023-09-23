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
                   Ui::pinSize({800, 600}) |
                   Ui::dialogLayer();
        });
}

} // namespace ImageViewer

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);
    Res<Media::Image> image = Error::invalidInput("No image provided");

    if (args.len() > 0) {
        auto url = try$(Url::parseUrlOrPath(args[0]));
        image = Media::loadImage(url);
    }

    return Ui::runApp(ctx, ImageViewer::app(image));
}

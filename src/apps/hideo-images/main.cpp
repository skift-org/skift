#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/scafold.h>

#include "app.h"

namespace Hideo::Images {

Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const &state) {
            auto titlebar = Ui::titlebar(Mdi::IMAGE, "Images");

            auto content = state.isEditor
                               ? editor(state)
                               : viewer(state);

            return Ui::vflow(titlebar, content | Ui::grow()) |
                   Ui::pinSize({800, 600}) |
                   Ui::dialogLayer();
        });
}

} // namespace Hideo::Images

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);
    Res<Media::Image> image = Error::invalidInput("No image provided");

    if (args.len()) {
        auto url = try$(Url::parseUrlOrPath(args[0]));
        image = Media::loadImage(url);
    }

    return Ui::runApp(ctx, Hideo::Images::app(image));
}

#include <hideo-base/scafold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>

#include "app.h"

namespace Hideo::Images {

Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const &state) {
            return scafold({
                .icon = Mdi::IMAGE,
                .title = "Images"s,
                .body = slot$(
                    state.isEditor
                        ? editor(state)
                        : viewer(state)
                ),
            });
        }
    );
}

} // namespace Hideo::Images

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);
    Res<Media::Image> image = Error::invalidInput("No image provided");

    if (args.len()) {
        auto url = try$(Mime::parseUrlOrPath(args[0]));
        image = Media::loadImage(url);
    }

    return Ui::runApp(ctx, Hideo::Images::app(image));
}

#include <karm-image/loader.h>
#include <karm-kira/scaffold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <mdi/image.h>

#include "app.h"

namespace Hideo::Images {

Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const& state) {
            return Kr::scaffold({
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

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = useArgs(ctx);
    Res<Image::Picture> image = Error::invalidInput("No image provided");

    if (args.len()) {
        auto url = co_try$(Mime::parseUrlOrPath(args[0]));
        image = Image::load(url);

        if (not image) {
            logError("Failed to load image: {}", image.none());
        }
    }

    co_return Ui::runApp(ctx, Hideo::Images::app(image));
}

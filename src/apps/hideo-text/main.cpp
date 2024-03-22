#include <hideo-base/alert.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

namespace Hideo::Text {

Ui::Child editor(Str text) {
    return Ui::bodyMedium(text) |
           Ui::spacing(6) |
           Ui::grow() |
           Ui::vscroll();
}

Ui::Child app(Res<String> text) {
    return Ui::scafold({
        .icon = Mdi::TEXT,
        .title = "Text",
        .startTools = slots$(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILE),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FOLDER),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS)
        ),
        .endTools = slots$(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::UNDO),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REDO)
        ),
        .body = [=] {
            return text ? editor(text.unwrap())
                        : alert("Unable to load text", text.none().msg());
        },
    });
}

Res<String> readAllUtf8(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    return Io::readAllUtf8(file);
}

} // namespace Hideo::Text

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);
    Res<String> text = Error::invalidInput("No text provided");
    auto url = try$(Mime::parseUrlOrPath(args[0]));
    if (args.len())
        text = Hideo::Text::readAllUtf8(url);
    return Ui::runApp(ctx, Hideo::Text::app(text));
}

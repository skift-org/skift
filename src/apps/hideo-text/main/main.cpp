#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = useArgs(ctx);
    Opt<Mime::Url> url;
    Res<String> text = Ok(""s);
    if (args.len()) {
        url = co_try$(Mime::parseUrlOrPath(args[0]));
        text = Sys::readAllUtf8(*url);
    }
    co_return Ui::runApp(ctx, Hideo::Text::app(url, text));
}

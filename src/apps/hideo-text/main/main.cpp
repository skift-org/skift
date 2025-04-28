#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/proc.h>

import Hideo.Text;
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = useArgs(ctx);
    Opt<Mime::Url> url;
    Res<String> text = Ok(""s);
    if (args.len()) {
        url = Mime::parseUrlOrPath(args[0], co_try$(Sys::pwd()));
        text = Sys::readAllUtf8(*url);
    }
    co_return co_await Ui::runAsync(ctx, Hideo::Text::app(url, text));
}

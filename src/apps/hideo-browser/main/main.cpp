#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <vaev-driver/fetcher.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? co_try$(Mime::parseUrlOrPath(args[0]))
                   : "about:start"_url;

    auto dom = Vaev::Driver::fetchDocument(url);

    co_return Ui::runApp(
        ctx,
        Hideo::Browser::app(url, dom)
    );
}

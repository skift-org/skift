#include <karm-sys/entry.h>
#include <karm-sys/proc.h>

import Karm.Ui;
import Vaev.Browser;
import Vaev.Engine;
import Karm.Http;
import Karm.Gc;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Ref::parseUrlOrPath(args[0], co_try$(Sys::pwd()))
                   : "about:start"_url;
    Gc::Heap heap;
    auto client = Http::defaultClient();
    client->userAgent = "Vaev-Browser/" stringify$(__ck_version_value) ""s;

    auto dom = co_await Vaev::Loader::fetchDocumentAsync(heap, *client, url);

    co_return co_await Ui::runAsync(
        ctx,
        Vaev::Browser::app(heap, *client, url, dom)
    );
}

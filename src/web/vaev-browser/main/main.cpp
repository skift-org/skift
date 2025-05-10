#include <karm-gc/heap.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>

import Karm.Ui;
import Vaev.Browser;
import Vaev.Loader;
import Karm.Http;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Mime::parseUrlOrPath(args[0], co_try$(Sys::pwd()))
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

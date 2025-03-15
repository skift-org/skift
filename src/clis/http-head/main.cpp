#include <karm-cli/args.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>

import Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto urlArg = Cli::operand<Str>("url"s, "URL to fetch"s, "localhost"s);

    Cli::Command cmd{
        "http-head"s,
        NONE,
        "Send a HEAD request to a URL and print the response headers"s,
        {
            urlArg,
        }
    };

    co_trya$(cmd.execAsync(ctx));

    auto url = Mime::parseUrlOrPath(urlArg, co_try$(Sys::pwd()));
    auto header = co_trya$(Http::headAsync(url))->header;
    co_try$(header.unparse(Sys::out()));

    co_return Ok();
}

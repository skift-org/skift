#include <karm-cli/args.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>

import Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto urlArg = Cli::operand<Str>("url"s, "URL to fetch"s, "localhost"s);

    Cli::Command cmd{
        "http-get"s,
        NONE,
        "Send a GET request to a URL and print the response body"s,
        {
            urlArg,
        }
    };

    co_trya$(cmd.execAsync(ctx));

    auto url = Mime::parseUrlOrPath(urlArg, co_try$(Sys::pwd()));
    auto resp = co_trya$(Http::getAsync(url));
    if (not resp->body)
        co_return Error::invalidData("no body in response");

    auto body = resp->body.take();

    auto adaptedOut = Aio::adapt(Sys::out());
    co_trya$(Aio::copyAsync(*body, adaptedOut));

    co_return Ok();
}

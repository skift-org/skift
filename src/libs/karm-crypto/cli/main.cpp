#include <karm-base/iter.h>
#include <karm-crypto/hash.h>
#include <karm-sys/entry-async.h>
#include <karm-sys/file.h>

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);

    auto url = co_try$(Mime::parseUrlOrPath(args[0]));
    auto file = co_try$(Sys::File::open(url));
    auto hash = co_try$(Crypto::fromName(args[0]));
    auto digest = co_try$(Crypto::digest(file, hash));

    Sys::println("{} {}", digest, args[1]);

    co_return Ok();
}

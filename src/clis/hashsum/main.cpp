#include <karm-base/iter.h>
#include <karm-crypto/hash.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);

    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto file = try$(Sys::File::open(url));
    auto hash = try$(Crypto::fromName(args[0]));
    auto digest = try$(Crypto::digest(file, hash));

    Sys::println("{} {}", digest, args[1]);

    return Ok();
}

#include <karm-base/iter.h>
#include <karm-hash/hash.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);

    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto file = try$(Sys::File::open(url));
    auto hash = try$(Hash::fromName(args[0]));
    auto digest = try$(Hash::digest(file, hash));

    Sys::println("{} {}", digest, args[1]);

    return Ok();
}

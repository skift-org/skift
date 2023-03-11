#include <karm-base/iter.h>
#include <karm-hash/hash.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>

Res<> entryPoint(CliArgs args) {
    auto file = try$(Sys::File::open(args[1]));
    auto hash = try$(Hash::fromName(args[0]));
    auto digest = try$(Hash::digest(file, hash));

    Sys::println("{} {}", digest, args[1]);

    return Ok();
}

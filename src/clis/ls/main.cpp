#include <karm-sys/dir.h>
#include <karm-sys/entry.h>

Res<> entryPoint(Sys::Ctx &) {
    auto url = try$(Url::parseUrlOrPath("."));
    auto dir = try$(Sys::Dir::open(url));
    for (auto const &entry : dir.entries())
        Sys::println(entry.name);
    return Ok();
}

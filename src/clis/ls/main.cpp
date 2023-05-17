#include <karm-main/main.h>
#include <karm-sys/dir.h>

Res<> entryPoint(Ctx &) {
    auto url = try$(Sys::parseUrlOrPath("."));
    auto dir = try$(Sys::Dir::open(url));
    for (auto const &entry : dir.entries()) {
        Sys::println(entry.name);
    }
    return Ok();
}

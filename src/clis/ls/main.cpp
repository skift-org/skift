#include <karm-sys/dir.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    auto url = co_try$(Mime::parseUrlOrPath("."));
    auto dir = co_try$(Sys::Dir::open(url));
    for (auto const &entry : dir.entries())
        Sys::println(entry.name);
    co_return Ok();
}

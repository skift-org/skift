#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/proc.h>

using namespace Karm;

Res<Ref::Mime> sniffFile(Ref::Url url) {
    auto file = try$(Sys::File::open(url));
    return file.sniff(true);
}

Async::Task<> entryPointAsync(Sys::Context& c) {
    auto& args = Sys::useArgs(c);

    for (usize i = 0; i < args.len(); i++) {
        auto url = Ref::parseUrlOrPath(args[i], co_try$(Sys::pwd()));
        Sys::println("file: {}", url);
        Sys::println("mime by sniffing url: {}", Ref::sniffSuffix(url.path.suffix()));
        Sys::println("mime by sniffing content: {}", sniffFile(url));
    }

    co_return Ok();
}

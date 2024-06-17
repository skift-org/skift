#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);
    if (args.len() != 2) {
        Sys::errln("usage: html2pdf <input.html> <output.pdf>\n");
        co_return Error::invalidInput();
    }

    co_return Ok();
}

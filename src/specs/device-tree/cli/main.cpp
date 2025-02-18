#include <device-tree/blob.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = useArgs(ctx);

    if (args.len() == 0) {
        co_return Error::invalidInput("Usage: dtb-dump <dtb-file>");
    }

    auto url = co_try$(Mime::parseUrlOrPath(args[0]));
    auto dtbFile = co_try$(Sys::File::open(url));
    auto dtbMem = co_try$(Sys::mmap().read().map(dtbFile));
    auto dtb = co_try$(DeviceTree::Blob::load(dtbMem.bytes()));

    if (not dtb.valid())
        co_return Error::invalidData("dtb is not a valid device tree");

    Sys::println("dtb is valid");
    co_return Ok();
}

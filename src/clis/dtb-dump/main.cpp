#include <device-tree/blob.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);

    if (args.len() == 0) {
        return Error::invalidInput("Usage: dtb-dump <dtb-file>");
    }

    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto dtbFile = try$(Sys::File::open(url));
    auto dtbMem = try$(Sys::mmap().read().map(dtbFile));
    auto dtb = try$(DeviceTree::Blob::load(dtbMem.bytes()));

    if (not dtb.valid()) {
        return Error::invalidData("dtb is not a valid device tree");
    }

    Sys::println("dtb is valid");
    return Ok();
}

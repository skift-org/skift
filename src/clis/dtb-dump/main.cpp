#include <device-tree/blob.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

Res<> entryPoint(CliArgs args) {
    if (args.len() == 0) {
        return Error{"Usage: dtb-dump <dtb-file>"};
    }
    Sys::File dtbFile = try$(Sys::File::open(args[0]));
    auto dtbMem = try$(Sys::mmap().read().map(dtbFile));
    DeviceTree::Blob dtb{dtbMem.bytes()};

    if (not dtb.valid()) {
        return Error{Error::INVALID_DATA, "dtb is not a valid device tree"};
    }

    Sys::println("dtb is valid");
    return Ok();
}

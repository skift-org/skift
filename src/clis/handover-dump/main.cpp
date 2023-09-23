#include <elf/image.h>
#include <handover/spec.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);

    if (args.len() == 0) {
        return Error::invalidInput("Usage: handover-dump <elf-file>");
    }

    auto url = try$(Url::parseUrlOrPath(args[0]));
    Sys::File kernelFile = try$(Sys::File::open(url));
    auto kernelMem = try$(Sys::mmap().read().map(kernelFile));
    Elf::Image kernelElf{kernelMem.bytes()};

    if (not kernelElf.valid()) {
        return Error::invalidData("kernel is not a valid ELF executable");
    }

    auto requests = try$(kernelElf.sectionByName(Handover::REQUEST_SECTION))
                        .slice<Handover::Request>();

    Sys::println("Requests:");
    for (auto const &request : requests) {
        Sys::println(" - {}", request.name());
    }

    Sys::println("Kernel is valid");
    return Ok();
}

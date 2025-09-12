#include <elf/image.h>
#include <handover/spec.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <karm-sys/proc.h>

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto& args = useArgs(ctx);

    if (args.len() == 0) {
        co_return Error::invalidInput("Usage: handover-dump <elf-file>");
    }

    auto url = Ref::parseUrlOrPath(args[0], co_try$(Sys::pwd()));
    auto kernelFile = co_try$(Sys::File::open(url));
    auto kernelMem = co_try$(Sys::mmap().read().map(kernelFile));
    Elf::Image kernelElf{kernelMem.bytes()};

    if (not kernelElf.valid())
        co_return Error::invalidData("kernel is not a valid ELF executable");

    auto requests = co_try$(kernelElf.sectionByName(Handover::REQUEST_SECTION))
                        .slice<Handover::Request>();

    Sys::println("Requests:");
    for (auto const& request : requests) {
        Sys::println(" - {}", request.name());
    }

    Sys::println("Kernel is valid");
    co_return Ok();
}

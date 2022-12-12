#include <elf/image.h>
#include <handover/spec.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

CliResult entryPoint(CliArgs args) {
    if (args.len() == 0) {
        return Error{"Usage: handover-dump <elf-file>"};
    }
    Sys::File kernelFile = try$(Sys::File::open(args[0]));
    auto kernelMem = try$(Sys::mmap().read().map(kernelFile));
    Elf::Image kernelElf{kernelMem.bytes()};

    if (not kernelElf.valid()) {
        return Error{Error::INVALID_DATA, "kernel is not a valid ELF executable"};
    }

    auto requests = try$(kernelElf.sectionByName(Handover::REQUEST_SECTION))
                        .slice<Handover::Request>();

    Sys::println("Requests:");
    for (auto const &request : requests) {
        Sys::println(" - {}", request.name());
    }

    Sys::println("Kernel is valid");
    return Cli::SUCCESS;
}

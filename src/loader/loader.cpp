#include <elf/image.h>
#include <handover/builder.h>
#include <karm-base/size.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "fw.h"
#include "loader.h"

namespace Loader {

Error load(Sys::Path kernelPath) {
    Sys::println("Loading kernel...");

    auto vmm = Fw::createVmm();

    // Setup payload builder
    auto payloadMem = try$(Sys::mmap().read().size(16 * 1024).mapMut());
    Handover::Builder payload{payloadMem.slice<uint8_t>()};
    payload.agent("loader");

    // Load kernel image
    Sys::File kerneFile = try$(Sys::File::open(kernelPath));
    auto kernelMem = try$(Sys::mmap().map(kerneFile));
    Elf::Image image{kernelMem.slice<uint8_t>()};

    // Setup the stack
    auto stackMap = try$(Sys::mmap().stack().size(16 * 1024).mapMut());

    payload.add(Handover::FILE, 0, kernelMem.prange());
    payload.add(Handover::SELF, 0, payloadMem.prange());
    payload.add(Handover::STACK, 0, stackMap.prange());

    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        size_t paddr = prog.vaddr() - Handover::KERNEL_BASE;
        size_t remaining = prog.memsz() - prog.len();
        memcpy((void *)paddr, prog.buf(), prog.len());
        memset((void *)(paddr + prog.len()), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, prog.memsz()});
    }

    auto requests = try$(image.sectionByName(Handover::REQUEST_SECTION)).slice<Handover::Request>();

    for (auto const &request : requests) {
        Sys::println("Handing over request: {}", request.name());
        // FIXME: Handle requests
    }

    try$(vmm->map({0, gib(2)}, {Handover::KERNEL_BASE, gib(2)}, Hal::Vmm::READ | Hal::Vmm::WRITE | Hal::Vmm::EXEC));
    try$(vmm->map({0, gib(4)}, {Handover::UPPER_HALF, gib(4)}, Hal::Vmm::READ | Hal::Vmm::WRITE));

    payloadMem.leak();
    kernelMem.leak();
    stackMap.leak();

    vmm->activate();

    Handover::EntryPoint entryPoint = (Handover::EntryPoint)(size_t)image.header().entry;
    entryPoint(Handover::COOLBOOT, payload.finalize());
    panic("Should not reach here");
}

} // namespace Loader

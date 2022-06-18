#include <elf/image.h>
#include <hal/mem.h>
#include <handover/builder.h>
#include <karm-base/align.h>
#include <karm-base/size.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "fw.h"
#include "loader.h"

namespace Loader {

void enterKernel(size_t entry, size_t payload, size_t stack, size_t vmm);

Error load(Sys::Path kernelPath) {

    Sys::println("Preparing payload...");
    auto payloadMem = try$(Sys::mmap().read().size(kib(16)).mapMut());
    Handover::Builder payload{payloadMem.slice<uint8_t>()};
    payload.add(Handover::SELF, 0, payloadMem.prange());

    Sys::println("Loading kernel file...");
    Sys::File kernelFile = try$(Sys::File::open(kernelPath));
    auto kernelMem = try$(Sys::mmap().map(kernelFile));
    Elf::Image image{kernelMem.slice<uint8_t>()};
    payload.add(Handover::FILE, 0, kernelMem.prange());

    if (!image.valid()) {
        return Error{Error::INVALID_DATA, "invalid kernel image"};
    }

    Sys::println("Setting up stack...");
    auto stackMap = try$(Sys::mmap().stack().size(kib(16)).mapMut());
    payload.add(Handover::STACK, 0, stackMap.prange());

    Sys::println("Loading kernel image...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        size_t paddr = prog.vaddr() - Handover::KERNEL_BASE;
        size_t memsz = Hal::pageAlignUp(prog.memsz());

        size_t remaining = prog.memsz() - prog.filez();
        memcpy((void *)paddr, prog.buf(), prog.filez());
        memset((void *)(paddr + prog.filez()), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, paddr + memsz});
    }

    Sys::println("Handling kernel requests...");
    auto requests = try$(image.sectionByName(Handover::REQUEST_SECTION)).slice<Handover::Request>();

    for (auto const &request : requests) {
        Sys::println(" - {}", request.name());
    }

    auto vmm = try$(Fw::createVmm());

    Sys::println("Mapping kernel...");
    try$(vmm->map(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, (Handover::KERNEL_BASE - Hal::PAGE_SIZE) + gib(2) - Hal::PAGE_SIZE},
        {0x1000, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Sys::println("Mapping upper half...");
    try$(vmm->map(
        {Handover::UPPER_HALF + 0x1000, Handover::UPPER_HALF + gib(4)},
        {0x1000, gib(4)},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Sys::println("Mapping loader image...");
    auto loaderImage = Fw::image();

    try$(vmm->map(
        Hal::VmmRange::identityMapped(loaderImage),
        loaderImage,
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Sys::println("Finishing up...");

    Sys::println("Entering kernel, see on the other side...");
    try$(Fw::finalizeHandover(payload));
    Fw::enterKernel(image.header().entry, payload.finalize(), Handover::KERNEL_BASE + (size_t)stackMap.end(), *vmm);
    panic("unreachable");
}

} // namespace Loader

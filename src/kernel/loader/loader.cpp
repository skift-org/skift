#include <elf/image.h>
#include <hal/mem.h>
#include <handover/builder.h>
#include <karm-base/align.h>
#include <karm-base/size.h>
#include <karm-debug/logger.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "fw.h"
#include "loader.h"

namespace Loader {

void enterKernel(size_t entry, size_t payload, size_t stack, size_t vmm);

Error load(Sys::Path kernelPath) {
    Debug::linfo("Preparing payload...");
    auto payloadMem = try$(Sys::mmap().read().size(kib(16)).mapMut());
    Debug::linfo("Payload at vaddr: 0x{x} paddr: 0x{x}", payloadMem.vaddr(), payloadMem.paddr());
    Handover::Builder payload{payloadMem.mutBytes()};

    payload.agent("skift loader");
    payload.add(Handover::SELF, 0, payloadMem.prange());

    Debug::linfo("Loading kernel file...");
    Sys::File kernelFile = try$(Sys::File::open(kernelPath));
    auto kernelMem = try$(Sys::mmap().map(kernelFile));
    Elf::Image image{kernelMem.bytes()};
    payload.add(Handover::FILE, 0, kernelMem.prange().as<USizeRange>());
    Debug::linfo("Kernel at vaddr: 0x{x} paddr: 0x{x}", kernelMem.vaddr(), kernelMem.paddr());

    if (!image.valid()) {
        return Error{Error::INVALID_DATA, "invalid kernel image"};
    }

    Debug::linfo("Setting up stack...");
    auto stackMap = try$(Sys::mmap().stack().size(kib(16)).mapMut());
    payload.add(Handover::STACK, 0, stackMap.prange());
    Debug::linfo("Stack at vaddr: 0x{x} paddr: 0x{x}", stackMap.vaddr(), stackMap.paddr());

    Debug::linfo("Loading kernel image...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        size_t paddr = prog.vaddr() - Handover::KERNEL_BASE;
        size_t memsz = Hal::pageAlignUp(prog.memsz());
        Debug::linfo("Loading segment: paddr=0x{x}, vaddr=0x{x}, memsz=0x{x}, filesz=0x{x}", paddr, prog.vaddr(), memsz, prog.filez());

        size_t remaining = prog.memsz() - prog.filez();
        memcpy((void *)paddr, prog.buf(), prog.filez());
        memset((void *)(paddr + prog.filez()), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, memsz});
    }

    Debug::linfo("Handling kernel requests...");
    auto maybeSection = image.sectionByName(Handover::REQUEST_SECTION);

    if (!maybeSection) {
        return Error{Error::INVALID_DATA, "missing .handover section"};
    }

    auto requests = try$(maybeSection).slice<Handover::Request>();

    for (auto const &request : requests) {
        Debug::linfo(" - {}", request.name());
    }

    auto vmm = try$(Fw::createVmm());

    Debug::linfo("Mapping kernel...");
    try$(vmm->map(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Debug::linfo("Mapping upper half...");
    try$(vmm->map(
        {Handover::UPPER_HALF + Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Debug::linfo("Mapping loader image...");
    auto loaderImage = Fw::imageRange();

    try$(vmm->map(
        Hal::identityMapped(loaderImage),
        loaderImage,
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    Debug::linfo("Finalizing and entering kernel, see you on the other side...");

    uintptr_t ip = image.header().entry;
    uintptr_t sp = Handover::KERNEL_BASE + (size_t)stackMap.mutBytes().end();
    Debug::linfo("ip:{x} sp:{x} payload:{}", ip, sp, (uintptr_t)&payload.finalize());

    try$(Fw::finalizeHandover(payload));
    Fw::enterKernel(ip, payload.finalize(), sp, *vmm);

    panic("unreachable");
}

} // namespace Loader

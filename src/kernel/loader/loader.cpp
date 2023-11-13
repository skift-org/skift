#include <elf/image.h>
#include <hal/mem.h>
#include <handover/builder.h>
#include <karm-base/align.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "fw.h"
#include "loader.h"

namespace Loader {

void enterKernel(usize entry, usize payload, usize stack, usize vmm);

Res<> loadEntry(Entry const &entry) {
    logInfo("loader: preparing payload...");
    auto payloadMem = try$(Sys::mmap().read().size(kib(16)).mapMut());
    logInfo("loader: payload at vaddr: 0x{x} paddr: 0x{x}", payloadMem.vaddr(), payloadMem.paddr());
    Handover::Builder payload{payloadMem.mutBytes()};

    payload.agent("opstart");
    payload.add(Handover::SELF, 0, payloadMem.prange());

    logInfo("loader: loading kernel file...");
    Sys::File kernelFile = try$(Sys::File::open(entry.kernel.url));
    auto kernelMem = try$(Sys::mmap().map(kernelFile));
    Elf::Image image{kernelMem.bytes()};
    payload.add(Handover::FILE, 0, kernelMem.prange().as<USizeRange>());
    logInfo("loader: kernel at vaddr: 0x{x} paddr: 0x{x}", kernelMem.vaddr(), kernelMem.paddr());

    if (not image.valid()) {
        logError("loader: invalid kernel image");
        return Error::invalidData("invalid kernel image");
    }

    logInfo("loader: setting up stack...");
    auto stackMap = try$(Sys::mmap().stack().size(Hal::PAGE_SIZE * 16).mapMut());
    payload.add(Handover::STACK, 0, stackMap.prange());
    logInfo("loader: stack at vaddr: 0x{x} paddr: 0x{x}", stackMap.vaddr(), stackMap.paddr());

    logInfo("loader: loading kernel image...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize paddr = prog.vaddr() - Handover::KERNEL_BASE;
        usize memsz = Hal::pageAlignUp(prog.memsz());
        logInfo("loader: loading segment: paddr=0x{x}, vaddr=0x{x}, memsz=0x{x}, filesz=0x{x}", paddr, prog.vaddr(), memsz, prog.filez());

        usize remaining = prog.memsz() - prog.filez();
        memcpy((void *)paddr, prog.buf(), prog.filez());
        memset((void *)(paddr + prog.filez()), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, memsz});
    }

    logInfo("loader: loading additional blobs...");
    for (auto const &blob : entry.blobs) {
        logInfo("loader: loading blob: {}", blob.url);

        auto blobFile = try$(Sys::File::open(blob.url));
        auto blobMem = try$(Sys::mmap().map(blobFile));
        auto blobRange = blobMem.prange();

        auto strId = payload.add(try$(blob.url.str()));
        auto propStr = try$(Json::stringify(blob.props));
        auto propsId = payload.add(propStr);

        payload.add({
            .tag = Handover::FILE,
            .start = blobRange.start,
            .size = blobRange.size,
            .file = {
                .name = (u32)strId,
                .meta = (u32)propsId,
            },
        });

        // NOTE: We leak the blob memory here because we don't want to
        //       want raii to unmap the blob before we enter the kernel.
        blobMem.leak();
    }

    logInfo("loader: handling kernel requests...");
    auto maybeSection = image.sectionByName(Handover::REQUEST_SECTION);

    if (not maybeSection) {
        logError("loader: missing .handover section");
        return Error::invalidData("missing .handover section");
    }

    auto requests = try$(maybeSection).slice<Handover::Request>();

    for (auto const &request : requests) {
        logInfo(" - {}", request.name());
    }

    auto vmm = try$(Fw::createVmm());

    logInfo("loader: mapping kernel...");
    try$(vmm->mapRange(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    logInfo("loader: mapping upper half...");
    try$(vmm->mapRange(
        {Handover::UPPER_HALF + Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    logInfo("loader: mapping loader image...");
    auto loaderImage = Fw::imageRange();

    try$(vmm->mapRange(
        Hal::identityMapped(loaderImage),
        loaderImage,
        Hal::Vmm::READ | Hal::Vmm::WRITE));

    logInfo("loader: finalizing and entering kernel, see you on the other side...");

    usize ip = image.header().entry;
    usize sp = Handover::KERNEL_BASE + (usize)stackMap.mutBytes().end();
    logInfo("loader: ip:{x} sp:{x} payload:{}", ip, sp, (usize)&payload.finalize());

    try$(Fw::finalizeHandover(payload));
    Fw::enterKernel(ip, payload.finalize(), sp, *vmm);

    panic("unreachable");
}

} // namespace Loader

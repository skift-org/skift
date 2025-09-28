import Karm.Core;
import Karm.Sys;

#include <elf/image.h>
#include <hal/mem.h>
#include <handover/builder.h>
#include <karm-logger/logger.h>

#include "fw.h"
#include "loader.h"

namespace Opstart {

void enterKernel(usize entry, usize payload, usize stack, usize vmm);

Res<> loadEntry(Entry const& entry) {
    logInfo("opstart: preparing payload...");
    auto payloadMem = try$(Sys::mmap().read().size(kib(16)).mapMut());
    logInfo("opstart: payload at vaddr: {p} paddr: {p}", payloadMem.vaddr(), payloadMem.paddr());
    Handover::Builder payload{payloadMem.mutBytes()};

    payload.agent("opstart");
    payload.add(Handover::SELF, 0, payloadMem.prange());

    logInfo("opstart: loading kernel file...");
    auto kernelFile = try$(Sys::File::open(entry.kernel.url));
    auto kernelMem = try$(Sys::mmap().map(kernelFile));
    Elf::Image image{kernelMem.bytes()};
    payload.add(Handover::FILE, 0, kernelMem.prange());
    logInfo("opstart: kernel at vaddr: {p} paddr: {p}", kernelMem.vaddr(), kernelMem.paddr());

    if (not image.valid()) {
        logError("opstart: invalid kernel image");
        return Error::invalidData("invalid kernel image");
    }

    logInfo("opstart: setting up stack...");
    auto stackMap = try$(Sys::mmap().stack().size(Hal::PAGE_SIZE * 16).mapMut());
    payload.add(Handover::STACK, 0, stackMap.prange());
    logInfo("opstart: stack at vaddr: {p} paddr: {p}", stackMap.vaddr(), stackMap.paddr());

    logInfo("opstart: loading kernel image...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize paddr = prog.vaddr() - Handover::KERNEL_BASE;
        usize memsz = Hal::pageAlignUp(prog.memsz());
        logInfo("opstart: loading segment: paddr={p}, vaddr={p}, memsz={p}, filesz={p}", paddr, prog.vaddr(), memsz, prog.filez());

        usize remaining = prog.memsz() - prog.filez();
        std::memcpy((void*)paddr, prog.buf(), prog.filez());
        std::memset((void*)(paddr + prog.filez()), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, memsz});
    }

    logInfo("opstart: loading additional blobs...");
    for (auto const& blob : entry.blobs) {
        logInfo("opstart: loading blob: {}", blob.url);

        auto blobFile = try$(Sys::File::open(blob.url));
        auto blobMem = try$(Sys::mmap().map(blobFile));
        auto blobRange = blobMem.prange();
        auto propStr = try$(Json::unparse(blob.props));

        payload.add({
            .tag = Handover::FILE,
            .start = blobRange.start,
            .size = blobRange.size,
            .file = {
                .name = (u32)payload.add(blob.url.str()),
                .meta = (u32)payload.add(propStr),
            },
        });

        // NOTE: We leak the blob memory here because we don't want to
        //       want raii to unmap the blob before we enter the kernel.
        blobMem.leak();
    }

    logInfo("opstart: handling kernel requests...");
    auto maybeSection = image.sectionByName(Handover::REQUEST_SECTION);

    if (not maybeSection) {
        logError("opstart: missing .handover section");
        return Error::invalidData("missing .handover section");
    }

    auto requests = try$(maybeSection).slice<Handover::Request>();

    for (auto const& request : requests) {
        logInfo(" - {}", request.name());
    }

    auto vmm = try$(Fw::createVmm());

    logInfo("opstart: mapping kernel...");
    try$(vmm->mapRange(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: mapping upper half...");
    try$(vmm->mapRange(
        {Handover::UPPER_HALF + Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, gib(4) - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: mapping boot-agent image...");
    auto loaderImage = Fw::imageRange();

    try$(vmm->mapRange(
        Hal::identityMapped(loaderImage),
        loaderImage,
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: finalizing and entering kernel, see you on the other side...");

    usize ip = image.header().entry;
    usize sp = Handover::KERNEL_BASE + (usize)stackMap.mutBytes().end();
    logInfo("opstart: ip:{x} sp:{x} payload:{}", ip, sp, (usize)&payload.finalize());

    try$(Fw::finalizeHandover(payload));
    Fw::enterKernel(ip, payload.finalize(), sp, *vmm);

    unreachable();
}

} // namespace Opstart

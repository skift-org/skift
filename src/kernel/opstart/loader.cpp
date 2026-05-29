module;

#include <hal/mem.h>
#include <hal/vmm.h>
#include <karm/macros>
#include <vaerk-handover/builder.h>

export module Opstart:loader;

import Karm.Core;
import Karm.Sys;
import Karm.Logger;
import Karm.Dl.Elf;

import :config;
import :arch;
import :mem;
import :handover;

using namespace Karm::Literals;

namespace Opstart {

void enterKernel(usize entry, usize payload, usize stack, usize vmm);

export Res<> loadEntry(Entry const& entry) {
    logInfo("opstart: preparing payload...");
    auto payloadMem = try$(Sys::mutMmap(NONE, {.size = 16_KiB}));
    logInfo("opstart: payload at vaddr: {p} paddr: {p}", payloadMem.vaddr(), payloadMem.paddr());
    Handover::Builder payload{payloadMem.mutBytes()};

    payload.agent("opstart");
    payload.add(Handover::SELF, 0, payloadMem.prange());

    logInfo("opstart: loading kernel file...");
    auto kernelFile = try$(Sys::File::open(entry.kernel.url));
    auto kernelMem = try$(Sys::mmap(kernelFile));
    Elf::ElfObject<Elf::CurrentAbi> object{kernelMem.bytes()};
    try$(object.validate());

    payload.add(Handover::BLOB, 0, kernelMem.prange());
    logInfo("opstart: kernel at vaddr: {p} paddr: {p}", kernelMem.vaddr(), kernelMem.paddr());

    logInfo("opstart: setting up stack...");
    auto stackMap = try$(Sys::mutMmap(NONE, {.options = Sys::MmapOption::STACK, .size = Hal::PAGE_SIZE * 16}));
    payload.add(Handover::STACK, 0, stackMap.prange());
    logInfo("opstart: stack at vaddr: {p} paddr: {p}", stackMap.vaddr(), stackMap.paddr());

    logInfo("opstart: loading kernel image...");
    for (Elf::ElfProgram prog : object.iterProgram()) {
        if (prog.type() != Elf::ElfPhdrType::PT_LOAD)
            continue;

        usize paddr = prog.p_vaddr - Handover::KERNEL_BASE;
        usize memsz = Hal::pageAlignUp(prog.p_memsz);
        logInfo("opstart: loading segment: paddr={p}, vaddr={p}, memsz={p}, filesz={p}", paddr, prog.p_vaddr, memsz, prog.p_filesz);

        usize remaining = prog.p_memsz - prog.p_filesz;
        std::memcpy((void*)paddr, prog.data.buf(), prog.p_filesz);
        std::memset((void*)(paddr + prog.p_filesz), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, memsz});
    }

    logInfo("opstart: loading additional blobs...");
    for (auto const& blob : entry.blobs) {
        logInfo("opstart: loading blob: {}", blob.url);

        auto blobFile = try$(Sys::File::open(blob.url));
        auto blobMem = try$(Sys::mmap(blobFile));
        auto blobRange = blobMem.prange();
        auto propStr = try$(Json::unparse(blob.props));

        payload.add({
            .tag = Handover::BLOB,
            .start = blobRange.start,
            .size = blobRange.size,
            .blob = {
                .name = (u32)payload.add(blob.url.str()),
                .meta = (u32)payload.add(propStr),
            },
        });

        // NOTE: We leak the blob memory here because we don't want to
        //       want raii to unmap the blob before we enter the kernel.
        blobMem.leak();
    }

    logInfo("opstart: handling kernel requests...");
    auto maybeSection = object.section(Handover::REQUEST_SECTION);

    if (not maybeSection) {
        logError("opstart: missing .handover section");
        return Error::invalidData("missing .handover section");
    }

    auto requests = try$(maybeSection).data.cast<Handover::Request>();

    for (auto const& request : requests) {
        logInfo(" - {}", request.name());
    }

    auto vmm = try$(createVmm());

    logInfo("opstart: mapping kernel...");
    try$(vmm->mapRange(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, 2_GiB - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, 2_GiB - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: mapping upper half...");
    try$(vmm->mapRange(
        {Handover::UPPER_HALF + Hal::PAGE_SIZE, 4_GiB - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, 4_GiB - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: mapping boot-agent image...");
    auto loaderImage = imageRange();

    try$(vmm->mapRange(
        Hal::identityMapped(loaderImage),
        loaderImage,
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    logInfo("opstart: finalizing and entering kernel, see you on the other side...");

    usize ip = object.header().e_entry;
    usize sp = Handover::KERNEL_BASE + (usize)stackMap.mutBytes().end();
    logInfo("opstart: ip:{x} sp:{x} payload:{}", ip, sp, (usize)&payload.finalize());

    try$(finalizeHandover(payload));
    Arch::enterKernel(ip, payload.finalize(), sp, *vmm);

    unreachable();
}

} // namespace Opstart

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

static constexpr bool DEBUG_ELF = false;

void enterKernel(usize entry, usize payload, usize stack, usize vmm);

export Res<> loadEntry(Entry const& entry) {
    logInfo("loading entry: {}", entry.name);

    auto payloadMem = try$(Sys::mutMmap(NONE, {.size = 16_KiB}));
    Handover::Builder payload{payloadMem.mutBytes()};

    payload.agent("opstart");
    payload.add(Handover::SELF, 0, payloadMem.prange());

    auto kernelFile = try$(Sys::File::open(entry.kernel.url));
    auto kernelMem = try$(Sys::mmap(kernelFile));
    Elf::ElfObject<Elf::CurrentAbi> object{kernelMem.bytes()};
    try$(object.validate());

    auto maybeSection = object.section(Handover::REQUEST_SECTION);
    auto requests = try$(maybeSection).data.cast<Handover::Request>();

    if (not maybeSection) {
        logError("opstart: missing .handover section");
        return Error::invalidData("missing .handover section");
    }

    // Kernel
    payload.add(Handover::BLOB, 0, kernelMem.prange());

    // Stack
    auto stackMap = try$(Sys::mutMmap(NONE, {.options = Sys::MmapOption::STACK, .size = Hal::PAGE_SIZE * 16}));
    payload.add(Handover::STACK, 0, stackMap.prange());

    // Loading elf
    for (Elf::ElfProgram prog : object.iterProgram()) {
        if (prog.type() != Elf::ElfPhdrType::PT_LOAD)
            continue;

        usize paddr = prog.p_vaddr - Handover::KERNEL_BASE;
        usize memsz = Hal::pageAlignUp(prog.p_memsz);
        logDebugIf(DEBUG_ELF, "opstart: loading segment: paddr={p}, vaddr={p}, memsz={p}, filesz={p}", paddr, prog.p_vaddr, memsz, prog.p_filesz);

        usize remaining = prog.p_memsz - prog.p_filesz;
        std::memcpy((void*)paddr, prog.data.buf(), prog.p_filesz);
        std::memset((void*)(paddr + prog.p_filesz), 0, remaining);

        payload.add(Handover::KERNEL, 0, {paddr, memsz});
    }

    auto vmm = try$(createVmm());

    // mapping kernel
    try$(vmm->mapRange(
        {Handover::KERNEL_BASE + Hal::PAGE_SIZE, 2_GiB - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, 2_GiB - Hal::PAGE_SIZE - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    // mapping upper half
    try$(vmm->mapRange(
        {Handover::UPPER_HALF + Hal::PAGE_SIZE, 4_GiB - Hal::PAGE_SIZE},
        {Hal::PAGE_SIZE, 4_GiB - Hal::PAGE_SIZE},
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    // boot agent image
    auto loaderImage = imageRange();

    try$(vmm->mapRange(
        Hal::identityMapped(loaderImage),
        loaderImage,
        {Hal::Vmm::READ, Hal::Vmm::WRITE}
    ));

    usize ip = object.header().e_entry;
    usize sp = Handover::KERNEL_BASE + (usize)stackMap.mutBytes().end();

    try$(finalizeHandover(entry, payload, requests));
    Arch::enterKernel(ip, payload.finalize(), sp, *vmm);

    unreachable();
}

} // namespace Opstart

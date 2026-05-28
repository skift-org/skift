module;

#include <hal-x86_64/vmm.h>
#include <vaerk-efi/base.h>

export module Opstart:mem;

namespace Opstart {

struct EfiPmm : Hal::Pmm {
    Res<Hal::PmmRange> allocRange(usize size, Flags<Hal::PmmFlags>) override {
        usize paddr = 0;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, size / Hal::PAGE_SIZE, &paddr));
        return Ok(Hal::PmmRange{paddr, size});
    }

    Res<> used(Hal::PmmRange range, Flags<Hal::PmmFlags>) override {
        usize paddr = range.start;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ADDRESS, Efi::MemoryType::LOADER_DATA, paddr / Hal::PAGE_SIZE, &paddr));
        return Ok();
    }

    Res<> free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages(range.start, range.size / Hal::PAGE_SIZE));
        return Ok();
    }
};

static EfiPmm pmm{};

export Res<Arc<Hal::Vmm>> createVmm() {
    usize upper = try$(pmm.allocRange(Hal::PAGE_SIZE, {})).start;
    std::memset(reinterpret_cast<void*>(upper), 0, Hal::PAGE_SIZE);
    return Ok(makeArc<x86_64::Vmm<>>(pmm, reinterpret_cast<x86_64::Pml<4>*>(upper)));
}

export Hal::PmmRange imageRange() {
    return {
        Hal::pageAlignDown(reinterpret_cast<usize>(Efi::li()->imageBase)),
        Hal::pageAlignUp(Efi::li()->imageSize),
    };
}

} // namespace Opstart
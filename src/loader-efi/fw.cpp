#include <efi/base.h>
#include <loader/fw.h>

#include <arch-x86_64/vmm.h>

namespace Loader::Fw {

struct EfiPmm : public Hal::Pmm {
    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags) override {
        size_t paddr;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::USER, size / 4096, &paddr));
        return Hal::PmmRange{paddr, size};
    }

    Error used(Hal::PmmRange, Hal::PmmFlags) override {
        notImplemented();
    }

    Error free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages((uint64_t)range.start, range.len() / 4096));
        return OK;
    }
};

static EfiPmm pmm{};

Strong<Hal::Vmm> createVmm() {
    return makeStrong<x86_64::Vmm>(pmm);
}

} // namespace Loader::Fw

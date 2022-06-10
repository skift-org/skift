#include <efi/base.h>
#include <loader/fw.h>

#include <arch-x86_64/vmm.h>

namespace Loader::Fw {

struct EfiPmm : public Hal::Pmm {
    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags) override {
        size_t paddr;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::USER, size / 4096, &paddr));
        return Hal::PmmRange{paddr, paddr + size};
    }

    Error used(Hal::PmmRange, Hal::PmmFlags) override {
        notImplemented();
    }

    Error free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages((uint64_t)range.start, range.size() / 4096));
        return OK;
    }
};

static EfiPmm pmm{};

Result<Strong<Hal::Vmm>> createVmm() {
    size_t upper = try$(pmm.alloc(x86_64::PAGE_SIZE, Hal::PmmFlags::NONE)).start;
    memset((void *)upper, 0, x86_64::PAGE_SIZE);
    return {makeStrong<x86_64::Vmm>(pmm, (x86_64::Pml<4> *)upper)};
}

Error finalizeHandover(Handover::Builder &) {
    size_t mmapSize = 0;
    size_t key = 0;
    size_t descSize = 0;
    uint32_t descVersion = 0;

    // NOTE: This is expectected to fail
    (void)Efi::bs()->getMemoryMap(&mmapSize, nullptr, &key, &descSize, &descVersion);

    try$(Efi::bs()->exitBootServices(Efi::imageHandle(), key));

    return OK;
}

} // namespace Loader::Fw

#include <efi/base.h>
#include <karm-base/align.h>
#include <loader/fw.h>

#include <arch-x86_64/vmm.h>

namespace Loader::Fw {

struct EfiPmm : public Hal::Pmm {
    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags) override {
        size_t paddr = 0;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, size / Hal::PAGE_SIZE, &paddr));
        return Hal::PmmRange{paddr, paddr + size};
    }

    Error used(Hal::PmmRange range, Hal::PmmFlags) override {
        size_t paddr = range.start;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ADDRESS, Efi::MemoryType::LOADER_DATA, paddr / Hal::PAGE_SIZE, &paddr));
        return OK;
    }

    Error free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages((uint64_t)range.start, range.size() / Hal::PAGE_SIZE));
        return OK;
    }
};

static EfiPmm pmm{};

Result<Strong<Hal::Vmm>> createVmm() {
    size_t upper = try$(pmm.alloc(Hal::PAGE_SIZE, Hal::PmmFlags::NIL)).start;
    memset((void *)upper, 0, Hal::PAGE_SIZE);
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

Hal::PmmRange image() {
    return {
        (size_t)Efi::li()->imageBase,
        Hal::pageAlignUp((size_t)Efi::li()->imageBase + Efi::li()->imageSize),
    };
}

extern "C" void __enterKernel(size_t entry, size_t payload, size_t stack, size_t vmm);

void enterKernel(size_t entry, Handover::Payload &payload, size_t stack, Hal::Vmm &vmm) {
    // Sys::println("__enterKernel: {x}", (size_t)__enterKernel);
    // Sys::println("entry: {x} payload: {x} stack: {x} vmm: {x}", entry, (size_t)&payload + Handover::KERNEL_BASE, stack, vmm.root());
    __enterKernel(entry, (size_t)&payload + Handover::KERNEL_BASE, stack, vmm.root());
}

} // namespace Loader::Fw

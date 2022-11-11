#include <efi/base.h>
#include <karm-base/align.h>
#include <loader/fw.h>

#include <hal-x86_64/vmm.h>

namespace Loader::Fw {

struct EfiPmm : public Hal::Pmm {
    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags) override {
        size_t paddr = 0;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, size / Hal::PAGE_SIZE, &paddr));
        return Hal::PmmRange{paddr, size};
    }

    Error used(Hal::PmmRange range, Hal::PmmFlags) override {
        size_t paddr = range.start;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ADDRESS, Efi::MemoryType::LOADER_DATA, paddr / Hal::PAGE_SIZE, &paddr));
        return OK;
    }

    Error free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages((uint64_t)range.start, range.size / Hal::PAGE_SIZE));
        return OK;
    }
};

static EfiPmm pmm{};

Result<Strong<Hal::Vmm>> createVmm() {
    size_t upper = try$(pmm.alloc(Hal::PAGE_SIZE, Hal::PmmFlags::NIL)).start;
    memset((void *)upper, 0, Hal::PAGE_SIZE);
    return {makeStrong<x86_64::Vmm<Hal::IdentityMapper>>(pmm, (x86_64::Pml<4> *)upper)};
}

Error parseGop(Handover::Builder &builder) {
    Efi::GraphicsOutputProtocol *gop = nullptr;
    try$(Efi::bs()->locateProtocol(&Efi::GraphicsOutputProtocol::UUID, nullptr, (void **)&gop));
    auto mode = gop->mode;

    if (mode->info->pixelFormat != Efi::PixelFormat::BLUE_GREEN_RED_RESERVED8_BIT_PER_COLOR) {
        return Error{"unsupported pixel format"};
    }

    Debug::linfo("gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);

    Handover::Record record = {
        .tag = Handover::FB,
        .start = (uint64_t)mode->frameBufferBase,
        .size = mode->info->pixelsPerScanLine * mode->info->verticalResolution * 4,
        .fb = {
            .width = (uint16_t)mode->info->horizontalResolution,
            .height = (uint16_t)mode->info->verticalResolution,
            .pitch = (uint16_t)(mode->info->pixelsPerScanLine * 4),
            .format = Handover::PixelFormat::BGRX8888,
        },
    };

    builder.add(record);

    return OK;
}

Error parseAcpi(Handover::Builder &builder) {
    auto *acpiTable = Efi::st()->lookupConfigurationTable(Efi::ConfigurationTable::ACPI_TABLE_UUID);
    if (!acpiTable)
        acpiTable = Efi::st()->lookupConfigurationTable(Efi::ConfigurationTable::ACPI2_TABLE_UUID);

    if (acpiTable) {
        builder.add(Handover::Tag::RSDP, 0, {(size_t)acpiTable->table, 0x1000});
        Debug::linfo("acpi: rsdp at {x}", (uintptr_t)acpiTable->table);
    }

    return OK;
}

Error parseMemoryMap(Handover::Builder &builder) {
    size_t mmapSize = 0;
    size_t key = 0;
    size_t descSize = 0;
    uint32_t descVersion = 0;

    // NOTE: This is expectected to fail
    (void)Efi::bs()->getMemoryMap(&mmapSize, nullptr, &key, &descSize, &descVersion);

    if (descSize < sizeof(Efi::MemoryDescriptor)) {
        return Error{Error::INVALID_DATA, "invalid memory descriptor size"};
    }

    // Allocating on the pool might creates at least one new descriptor
    // https://stackoverflow.com/questions/39407280/uefi-simple-example-of-using-exitbootservices-with-gnu-efi
    mmapSize += 2 * descSize;

    auto *buffer = new Byte[mmapSize];
    try$(Efi::bs()->getMemoryMap(&mmapSize, (Efi::MemoryDescriptor *)buffer, &key, &descSize, &descVersion));

    size_t descLen = mmapSize / descSize;
    for (size_t i = 0; i < descLen; i++) {
        auto &desc = *(Efi::MemoryDescriptor *)(buffer + i * descSize);

        size_t start = desc.physicalStart;
        size_t size = desc.numberOfPages * Hal::PAGE_SIZE;

        switch (desc.type) {
        case Efi::MemoryType::LOADER_CODE:
        case Efi::MemoryType::LOADER_DATA:
        case Efi::MemoryType::BOOT_SERVICES_CODE:
        case Efi::MemoryType::BOOT_SERVICES_DATA:
        case Efi::MemoryType::RUNTIME_SERVICES_CODE:
        case Efi::MemoryType::RUNTIME_SERVICES_DATA:
            builder.add(Handover::Tag::LOADER, 0, {start, size});
            break;

        case Efi::MemoryType::CONVENTIONAL_MEMORY:
            builder.add(Handover::Tag::FREE, 0, {start, size});
            break;

        default:
            builder.add(Handover::Tag::RESERVED, 0, {start, size});
            break;
        }
    }

    try$(Efi::bs()->exitBootServices(Efi::imageHandle(), key));

    return OK;
}

Error finalizeHandover(Handover::Builder &builder) {
    try$(parseGop(builder));
    try$(parseAcpi(builder));
    try$(parseMemoryMap(builder));

    return OK;
}

Hal::PmmRange imageRange() {
    return {
        Hal::pageAlignDown((size_t)Efi::li()->imageBase),
        Hal::pageAlignUp((size_t)Efi::li()->imageSize),
    };
}

// Implemented in kernel-entry.s
extern "C" void __enterKernel(size_t entry, size_t payload, size_t stack, size_t vmm);

void enterKernel(size_t entry, Handover::Payload &payload, size_t stack, Hal::Vmm &vmm) {
    __enterKernel(entry, (size_t)&payload + Handover::KERNEL_BASE, stack, vmm.root());
}

} // namespace Loader::Fw

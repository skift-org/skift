#include <efi/base.h>
#include <hal-x86_64/vmm.h>
#include <karm-base/align.h>
#include <opstart/fw.h>

namespace Opstart::Fw {

struct EfiPmm : public Hal::Pmm {
    Res<Hal::PmmRange> allocRange(usize size, Hal::PmmFlags) override {
        usize paddr = 0;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, size / Hal::PAGE_SIZE, &paddr));
        return Ok(Hal::PmmRange{paddr, size});
    }

    Res<> used(Hal::PmmRange range, Hal::PmmFlags) override {
        usize paddr = range.start;
        try$(Efi::bs()->allocatePages(Efi::AllocateType::ADDRESS, Efi::MemoryType::LOADER_DATA, paddr / Hal::PAGE_SIZE, &paddr));
        return Ok();
    }

    Res<> free(Hal::PmmRange range) override {
        try$(Efi::bs()->freePages((u64)range.start, range.size / Hal::PAGE_SIZE));
        return Ok();
    }
};

static EfiPmm pmm{};

Res<Strong<Hal::Vmm>> createVmm() {
    usize upper = try$(pmm.allocRange(Hal::PAGE_SIZE, Hal::PmmFlags::NONE)).start;
    memset((void *)upper, 0, Hal::PAGE_SIZE);
    return Ok(makeStrong<x86_64::Vmm<>>(pmm, (x86_64::Pml<4> *)upper));
}

Res<> parseGop(Handover::Builder &builder) {
    auto *gop = try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
    auto *mode = gop->mode;

    if (mode->info->pixelFormat != Efi::PixelFormat::BLUE_GREEN_RED_RESERVED8_BIT_PER_COLOR) {
        return Error::invalidInput("unsupported pixel format");
    }

    logInfo("efi: gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);

    Handover::Record record = {
        .tag = Handover::FB,
        .start = (u64)mode->frameBufferBase,
        .size = mode->info->pixelsPerScanLine * mode->info->verticalResolution * 4,
        .fb = {
            .width = (u16)mode->info->horizontalResolution,
            .height = (u16)mode->info->verticalResolution,
            .pitch = (u16)(mode->info->pixelsPerScanLine * 4),
            .format = Handover::PixelFormat::BGRX8888,
        },
    };

    builder.add(record);

    return Ok();
}

Res<> parseAcpi(Handover::Builder &builder) {
    auto *acpiTable = Efi::st()->lookupConfigurationTable(Efi::ConfigurationTable::ACPI_TABLE_GUID);
    if (not acpiTable)
        acpiTable = Efi::st()->lookupConfigurationTable(Efi::ConfigurationTable::ACPI2_TABLE_GUID);

    if (acpiTable) {
        builder.add(Handover::Tag::RSDP, 0, {(usize)acpiTable->table, 0x1000});
        logInfo("efi: acpi: rsdp at {x}", (usize)acpiTable->table);
    }

    return Ok();
}

Res<> parseMemoryMap(Handover::Builder &builder) {
    usize mmapSize = 0;
    usize key = 0;
    usize descSize = 0;
    u32 descVersion = 0;

    // NOTE: This is expectected to fail
    (void)Efi::bs()->getMemoryMap(&mmapSize, nullptr, &key, &descSize, &descVersion);

    if (descSize < sizeof(Efi::MemoryDescriptor)) {
        return Error::invalidInput("invalid memory descriptor size");
    }

    // NOTE: Allocating on the pool might creates at least one new descriptor
    // https://stackoverflow.com/questions/39407280/uefi-simple-example-of-using-exitbootservices-with-gnu-efi
    mmapSize += 2 * descSize;

    auto buf = Buf<Byte>::init(mmapSize);
    try$(Efi::bs()->getMemoryMap(&mmapSize, (Efi::MemoryDescriptor *)buf.buf(), &key, &descSize, &descVersion));

    usize descLen = mmapSize / descSize;
    for (usize i = 0; i < descLen; i++) {
        auto &desc = *(Efi::MemoryDescriptor *)(buf.buf() + i * descSize);

        usize start = desc.physicalStart;
        usize size = desc.numberOfPages * Hal::PAGE_SIZE;

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

    // NOTE: At this point we can't free the buffer since we're not in boot services anymore
    buf.leak();

    return Ok();
}

Res<> finalizeHandover(Handover::Builder &builder) {
    try$(parseGop(builder));
    try$(parseAcpi(builder));
    try$(parseMemoryMap(builder));

    return Ok();
}

Hal::PmmRange imageRange() {
    return {
        Hal::pageAlignDown((usize)Efi::li()->imageBase),
        Hal::pageAlignUp((usize)Efi::li()->imageSize),
    };
}

// Implemented in kernel-entry.s
extern "C" void __enterKernel(usize entry, usize payload, usize stack, usize vmm);

void enterKernel(usize entry, Handover::Payload &payload, usize stack, Hal::Vmm &vmm) {
    __enterKernel(entry, (usize)&payload + Handover::KERNEL_BASE, stack, vmm.root());
}

} // namespace Opstart::Fw

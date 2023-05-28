#include <elf/image.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-base/size.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

Res<> loadService(Str url) {
    logInfo("system: loading service '{}'...", url);

    auto &handover = useHandover();
    auto *elf = handover.fileByName(url.buf());
    if (not elf) {
        logError("Failed to find shell app");
        return Error::invalidFilename();
    }
    logInfo("system: mapping elf...");
    auto elfVmo = try$(Hj::Vmo::create(Hj::ROOT, elf->start, elf->size, Hj::VmoFlags::DMA));
    try$(elfVmo.label("elf"));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logInfo("system: creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    Elf::Image image{elfRange.bytes()};
    if (not image.valid()) {
        logError("Invalid elf");
        return Error::invalidInput();
    }

    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);
        if ((prog.flags() & Elf::ProgramFlags::WRITE) == Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, size, Hj::VmoFlags::UPPER));
            try$(sectionVmo.label("elf-writeable"));

            // Map the VMO into the current address space so we can copy the data into it.
            auto sectionRange = try$(Hj::map(sectionVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(elfSpace.map(prog.vaddr(), sectionVmo, 0, size, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(elfSpace.map(prog.vaddr(), elfVmo, prog.offset(), size, Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfo("system: mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, kib(16), Hj::VmoFlags::UPPER));
    try$(stackVmo.label("elf-stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, kib(16), Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("system: creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, domain, elfSpace));
    try$(task.label("service"));

    logInfo("system: starting the task...");
    try$(task.start(image.header().entry, stackRange.end(), {}));

    return Ok();
}

Res<> entryPoint(Ctx &ctx) {
    try$(Hj::Task::self().label("system"));

    auto &handover = useHandover(ctx);

    auto *fb = handover.findTag(Handover::Tag::FB);
    auto fbVmo = try$(Hj::Vmo::create(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
    try$(fbVmo.label("framebuffer"));
    auto fbRange = try$(Hj::map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    Gfx::MutPixels pixels = {
        (void *)fbRange.mutBytes().buf(),
        {fb->fb.width, fb->fb.height},
        fb->fb.pitch,
        Gfx::BGRA8888,
    };

    pixels.clear(Gfx::GREEN);
    pixels.clip(pixels.bound().shrink(10)).clear(Gfx::ZINC900);
    logInfo("Hello from system server!");

    try$(loadService("bundle://shell-app/_bin"));

    return Ok();
}

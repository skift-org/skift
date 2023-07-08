#include <elf/image.h>
#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-base/size.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

Res<> loadService(Ctx &ctx, Sys::Url url) {
    logInfo("system: loading service '{}'...", url);

    auto &handover = useHandover(ctx);
    auto urlStr = try$(url.str());
    auto *elf = handover.fileByName(urlStr.buf());
    if (not elf) {
        logError("system: service '{}' not found", url);
        return Error::invalidFilename();
    }

    logInfo("system: mapping elf...");
    auto elfVmo = try$(Hj::Vmo::create(Hj::ROOT, elf->start, elf->size, Hj::VmoFlags::DMA));
    try$(elfVmo.label("elf-shared"));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logInfo("system: creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    logInfo("system: validating elf...");
    Elf::Image image{elfRange.bytes()};
    if (not image.valid()) {
        logError("Invalid elf");
        return Error::invalidInput();
    }

    logInfo("system: mapping the elf...");

    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);
        logInfo("system: mapping section: {x}-{x}", prog.vaddr(), prog.vaddr() + size);
        if ((prog.flags() & Elf::ProgramFlags::WRITE) == Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, size, Hj::VmoFlags::UPPER));
            try$(sectionVmo.label("elf-writeable"));
            auto sectionRange = try$(Hj::map(sectionVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(elfSpace.map(prog.vaddr(), sectionVmo, 0, size, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(elfSpace.map(prog.vaddr(), elfVmo, prog.offset(), size, Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfo("system: mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, kib(64), Hj::VmoFlags::UPPER));
    try$(stackVmo.label("stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("system: creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, domain, elfSpace));

    logInfo("system: mapping handover...");
    auto const *handoverRecord = handover.findTag(Handover::Tag::SELF);
    auto handoverVmo = try$(Hj::Vmo::create(Hj::ROOT, handoverRecord->start, handoverRecord->size, Hj::VmoFlags::DMA));
    try$(handoverVmo.label("handover"));
    auto handoverVrange = try$(elfSpace.map(0, handoverVmo, 0, 0, Hj::MapFlags::READ));

    // NOTE: -2 because we want the name of the service, not the binary
    try$(task.label(url.host));

    logInfo("system: starting the task...");
    try$(task.start(image.header().entry, stackRange.end(), {handoverVrange.start}));

    return Ok();
}

Res<> displayBootscreen(Ctx &ctx) {
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

    Gfx::Context g;
    g.begin(pixels);

    g.clear(Gfx::ZINC950);
    g.origin(pixels.size() / 2);

    g.evalSvg(
#include "skift.path"
    );
    g.fill(Gfx::WHITE);

    g.end();

    return Ok();
}

Res<> entryPoint(Ctx &ctx) {
    try$(Hj::Task::self().label("system"));

    try$(displayBootscreen(ctx));
    try$(loadService(ctx, "bundle://device-srv/_bin"_url));
    try$(loadService(ctx, "bundle://shell-app/_bin"_url));

    return Ok();
}

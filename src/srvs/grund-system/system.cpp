#include <elf/image.h>
#include <handover/hook.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>

#include "system.h"

namespace Grund::System {

static constexpr bool DEBUG_TASK = false;
static constexpr bool DEBUG_ELF = false;

Res<Strong<Service>> Service::load(Sys::Context &ctx, Str id) {
    logInfo("system: loading service '{}'...", id);

    auto &handover = useHandover(ctx);
    auto urlStr = try$(Io::format("bundle://{}/_bin", id));
    auto *elf = handover.fileByName(urlStr.buf());
    if (not elf)
        return Error::invalidFilename("service not found");

    logInfoIf(DEBUG_ELF, "system: mapping elf...");
    auto elfVmo = try$(Hj::Vmo::create(Hj::ROOT, elf->start, elf->size, Hj::VmoFlags::DMA));
    try$(elfVmo.label("elf-shared"));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logInfoIf(DEBUG_ELF, "system: creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    logInfoIf(DEBUG_ELF, "system: validating elf...");
    Elf::Image image{elfRange.bytes()};
    if (not image.valid())
        return Error::invalidInput("invalid elf");

    logInfoIf(DEBUG_ELF, "system: mapping the elf...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD)
            continue;

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);
        logInfoIf(DEBUG_ELF, "system: mapping section: {x}-{x}", prog.vaddr(), prog.vaddr() + size);
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

    logInfoIf(DEBUG_ELF, "system: mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, kib(64), Hj::VmoFlags::UPPER));
    try$(stackVmo.label("stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfoIf(DEBUG_TASK, "system: creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, domain, elfSpace));
    try$(task.label(id));

    logInfoIf(DEBUG_TASK, "system: mapping handover...");
    auto const *handoverRecord = handover.findTag(Handover::Tag::SELF);
    auto handoverVmo = try$(Hj::Vmo::create(Hj::ROOT, handoverRecord->start, handoverRecord->size, Hj::VmoFlags::DMA));
    try$(handoverVmo.label("handover"));
    auto handoverVrange = try$(elfSpace.map(0, handoverVmo, 0, 0, Hj::MapFlags::READ));

    logInfoIf(DEBUG_TASK, "system: creating communication channels...");
    logInfoIf(DEBUG_TASK, "system: in...");
    auto in = try$(Hj::Channel::create(Hj::Domain::self(), 4096, 16));
    try$(in.label("in"));
    auto inCap = try$(domain.attach(in));

    logInfoIf(DEBUG_TASK, "system: out...");
    auto out = try$(Hj::Channel::create(Hj::Domain::self(), 512, 16));
    try$(out.label("out"));
    auto outCap = try$(domain.attach(out));

    logInfoIf(DEBUG_TASK, "system: starting the task...");
    try$(task.start(
        image.header().entry,
        stackRange.end(),
        {
            handoverVrange.start,
            inCap.slot(),
            outCap.slot(),
        }
    ));

    return Ok(makeStrong<Service>(
        std::move(task),
        std::move(in),
        std::move(out)
    ));
}

} // namespace Grund::System

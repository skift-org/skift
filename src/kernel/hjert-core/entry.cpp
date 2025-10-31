import Karm.Core;
import Karm.Logger;
import Vaerk.Elf;

#include <handover/entry.h>

#include "arch.h"
#include "cpu.h"
#include "domain.h"
#include "mem.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

Res<> enterUserspace(Handover::Payload& payload) {
    auto const* record = payload.fileByName("bundle://strata-bus/_bin");
    if (not record) {
        logInfo("handover: no init file");
        return Error::invalidInput("No init file");
    }

    auto space = try$(Space::create());
    space->label("init-space");

    logInfo("entry: mapping elf...");
    auto elfVmo = try$(Vmo::makeDma(record->range<Hal::DmaRange>()));
    elfVmo->label("elf-shared");
    auto elfRange = try$(kmm().pmm2Kmm(elfVmo->range()));
    Vaerk::Elf::Image image{elfRange.bytes()};

    if (not image.valid()) {
        logInfo("entry: invalid elf");
        return Error::invalidInput("Invalid elf");
    }

    for (auto const& prog : image.programs()) {
        if (prog.type() != Vaerk::Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);

        if (prog.flags().has(Vaerk::Elf::ProgramFlags::WRITE)) {
            auto sectionVmo = try$(Vmo::alloc(size, Hj::VmoFlags::UPPER));
            sectionVmo->label("elf-writeable");
            auto sectionRange = try$(kmm().pmm2Kmm(sectionVmo->range()));
            logInfo("entry: mapping section: {x}-{x}", sectionRange.start, sectionRange.end());
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(space->map({prog.vaddr(), size}, sectionVmo, 0, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        } else {
            try$(space->map({prog.vaddr(), size}, elfVmo, prog.offset(), {Hj::MapFlags::READ, Hj::MapFlags::EXEC}));
        }
    }

    logInfo("entry: mapping handover...");
    auto handoverBase = ((usize)&payload) - Handover::KERNEL_BASE;
    auto handoverSize = payload.size;
    auto handoverVmo = try$(Vmo::makeDma({handoverBase, handoverSize}));
    handoverVmo->label("handover");
    auto handoverRange = try$(space->map({}, handoverVmo, 0, Hj::MapFlags::READ));

    logInfo("entry: mapping stack...");
    auto stackVmo = try$(Vmo::alloc(kib(64), Hj::VmoFlags::UPPER));
    stackVmo->label("stack");
    auto stackRange = try$(space->map({}, stackVmo, 0, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
    logInfo("entry: stack: {x}-{x}", stackRange.start, stackRange.end());

    logInfo("entry: creating task...");
    auto domain = try$(Domain::create());
    domain->label("init-domain");
    auto task = try$(Task::create(Mode::USER, space, domain));
    task->label("init-task");

    try$(task->ready(image.header().entry, stackRange.end(), {handoverRange.start}));
    try$(globalSched().enqueue(task));

    return Ok();
}

Res<> init(u64 magic, Handover::Payload& payload) {
    try$(Arch::init(payload));

    logInfo("hjert " stringify$(__ck_version_value));

    if (not Handover::valid(magic, payload)) {
        logInfo("handover: invalid");
        return Error::invalidInput("invalid handover payload");
    }
    logInfo("handover: valid");
    logInfo("handover: agent: '{}'", payload.agentName());

    try$(initMem(payload));
    try$(initSched(payload));

    logInfo("entry: everything is ready, enabling interrupts...");
    Arch::globalCpu().retainEnable();
    Arch::globalCpu().enableInterrupts();

    logInfo("entry: entering userspace...");
    try$(enterUserspace(payload));

    logInfo("entry: entering idle loop...");
    Task::self().label("idle");
    Task::self().enter(Mode::IDLE);
    while (true)
        Arch::globalCpu().relaxe();
}

} // namespace Hjert::Core

// MARK: Handover Entry Point ------ -------------------------------------------

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles()
);

void __panicHandler(PanicKind kind, char const* buf) {
    if (kind == PanicKind::PANIC) {
        (void)Io::format(Hjert::Arch::globalOut(), "PANIC: {}\n", buf);
        Hjert::Arch::stop();
        __builtin_unreachable();
    } else {
        (void)Io::format(Hjert::Arch::globalOut(), "DEBUG: {}\n", buf);
    }
}

Res<> entryPoint(u64 magic, Handover::Payload& payload) {
    Karm::registerPanicHandler(__panicHandler);
    return Hjert::Core::init(magic, payload);
}

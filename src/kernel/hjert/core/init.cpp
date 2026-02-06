module;

#include <ce-bootfs/bootfs.h>
#include <hal/io.h>
#include <hal/mem.h>
#include <hal/vmm.h>
#include <karm/macros>
#include <vaerk-handover/spec.h>

export module Hjert.Core:init;

import Vaerk.Elf;
import :arch;
import :cpu;
import :sched;
import :space;

namespace Hjert::Core {

Res<Arc<Vmo>> _locateInit(Handover::Payload& payload, Str path) {
    logInfo("locating {#}...", path);

    auto const* record = payload.fileByName("file:/skift/init.bootfs");
    if (not record) {
        logError("handover: could not find bootfs");
        return Error::invalidInput("could not find bootfs");
    }

    auto bootfsRange = try$(kmm().pmm2Kmm(record->range<Hal::PmmRange>()));
    auto* bootfs = reinterpret_cast<bootfs_header_t*>(bootfsRange.mutBytes().buf());

    auto initFile = bootfs_open(bootfs, path.buf(), path.len());
    if (not initFile.length) {
        logError("handover: could not find init");
        return Error::invalidInput("could not find init");
    }

    Hal::DmaRange elfRange = {
        record->start + initFile.dirent->offset,
        alignUp(initFile.dirent->length, Hal::PAGE_SIZE),
    };

    return Vmo::makeDma(elfRange);
}

Res<> enterUserspace(Handover::Payload& payload) {
    auto space = try$(Space::create());
    space->label("init-space");

    logInfo("entry: mapping elf...");
    auto elfVmo = try$(_locateInit(payload, "bundles/strata-cm/_bin"));
    elfVmo->label("elf-shared");
    auto elfRange = try$(kmm().pmm2Kmm(elfVmo->range()));
    Vaerk::Elf::Image image{elfRange.bytes()};

    if (not image.valid()) {
        logInfo("entry: invalid elf");
        return Error::invalidInput("Invalid elf");
    }

    for (auto prog : image.programs()) {
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
    auto stackVmo = try$(Vmo::alloc(64_KiB, Hj::VmoFlags::UPPER));
    stackVmo->label("stack");
    auto stackRange = try$(space->map({}, stackVmo, 0, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
    logInfo("entry: stack: {x}-{x}", stackRange.start, stackRange.end());

    logInfo("entry: creating task...");
    auto domain = try$(Domain::create());
    domain->label("init-domain");
    auto task = try$(Task::create(Hj::Mode::USER, space, domain));
    task->label("init-task");

    try$(task->ready(image.header().entry, stackRange.end(), {handoverRange.start}));
    try$(globalSched().enqueue(task));

    return Ok();
}

export Res<> init(u64 magic, Handover::Payload& payload) {
    try$(Arch::init(payload));

    logInfo("hjert " stringify$(__ck_version_value));

    if (not Handover::valid(magic, payload)) {
        logInfo("handover: invalid");
        return Error::invalidInput("invalid handover payload");
    }
    logInfo("handover: valid");
    logInfo("handover: agent: '{}'", payload.agentName());

    try$(initMem(payload));
    try$(initSched());

    logInfo("entry: everything is ready, enabling interrupts...");
    Arch::globalCpu().retainEnable();
    Arch::globalCpu().enableInterrupts();

    logInfo("entry: entering userspace...");
    try$(enterUserspace(payload));

    logInfo("entry: entering idle loop...");
    globalSched().currentTask().label("idle");
    globalSched().currentTask().enter(Hj::Mode::IDLE);
    while (true)
        Arch::globalCpu().relaxe();
}

} // namespace Hjert::Core

#include <elf/image.h>
#include <handover/main.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>

#include "arch.h"
#include "cpu.h"
#include "mem.h"
#include "sched.h"

namespace Hjert::Core {

void splash() {
    logInfo(" _  _  _         _");
    logInfo("| || |(_)___ _ _| |_");
    logInfo("| __ || / -_) '_|  _|");
    logInfo("|_||_|/ \\___|_|  \\__|");
    logInfo("    |__/");
}

Res<> validateAndDump(u64 magic, Handover::Payload &payload) {
    if (not Handover::valid(magic, payload)) {
        logInfo("entry: handover: invalid");
        return Error::invalidInput("Invalid handover payload");
    }

    logInfo("entry: handover: valid");
    logInfo("entry: handover: agent: '{}'", payload.agentName());

    usize totalFree = 0;
    logInfo("entry: dumping handover records...");
    for (auto const &record : payload) {
        logInfo(
            " - {} {x}-{x} ({}kib)",
            record.name(),
            record.start,
            record.end(),
            record.size / 1024);

        if (record.tag == Handover::FREE) {
            totalFree += record.size;
        }

        if (record.tag == Handover::FILE) {
            auto name = payload.stringAt(record.file.name);
            logInfo("   - file: '{}'", name);
        }
    }

    logInfo("entry: handover: total free: {}mib", toMib(totalFree));

    return Ok();
}

Res<> enterUserspace(Handover::Payload &payload) {
    auto const *record = payload.fileByName("bundle://system-srv/_bin");
    if (not record) {
        logInfo("entry: handover: no init file");
        return Error::invalidInput("No init file");
    }

    auto space = try$(Space::create());
    space->label("init-space");

    logInfo("entry: mapping elf...");
    auto elfVmo = try$(VNode::makeDma(record->range<Hal::DmaRange>()));
    elfVmo->label("elf-readonly");
    auto elfRange = try$(kmm().pmm2Kmm(elfVmo->range()));
    Elf::Image image{elfRange.bytes()};

    if (not image.valid()) {
        logInfo("entry: invalid elf");
        return Error::invalidInput("Invalid elf");
    }

    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);

        if ((prog.flags() & Elf::ProgramFlags::WRITE) == Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(VNode::alloc(size, Hj::VmoFlags::UPPER));
            sectionVmo->label("elf-writeable");
            auto sectionRange = try$(kmm().pmm2Kmm(sectionVmo->range()));
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(space->map({prog.vaddr(), size}, sectionVmo, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(space->map({prog.vaddr(), size}, elfVmo, prog.offset(), Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfo("entry: mapping handover...");
    auto handoverBase = ((usize)&payload) - Handover::KERNEL_BASE;
    auto handoverSize = payload.size;
    auto handoverVmo = try$(VNode::makeDma({handoverBase, handoverSize}));
    handoverVmo->label("handover");
    auto handoverRange = try$(space->map({}, handoverVmo, 0, Hj::MapFlags::READ));

    logInfo("entry: mapping stack...");
    auto STACK_SIZE = kib(16);
    auto stackVmo = try$(VNode::alloc(STACK_SIZE, Hj::VmoFlags::UPPER));
    stackVmo->label("stack");
    auto stackRange = try$(space->map({}, stackVmo, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("entry: creating task...");
    auto domain = try$(Domain::create());
    domain->label("init-domain");
    auto task = try$(Task::create(TaskMode::USER, space, domain));
    task->label("init-task");
    try$(Sched::instance().start(task, image.header().entry, stackRange.end(), {handoverRange.start}));

    return Ok();
}

Res<> init(u64 magic, Handover::Payload &payload) {
    try$(Arch::init(payload));

    splash();
    try$(validateAndDump(magic, payload));

    try$(Mem::init(payload));
    try$(Sched::init(payload));

    logInfo("entry: everything is ready, enabling interrupts...");
    Arch::cpu().retainEnable();
    Arch::cpu().enableInterrupts();

    logInfo("entry: entering userspace...");
    try$(enterUserspace(payload));

    logInfo("entry: entering idle loop...");
    Task::self().label("idle");
    Task::self().enterIdleMode();
    while (true)
        Arch::cpu().relaxe();
}

} // namespace Hjert::Core

/* --- Handover Entry Point ------ ------------------------------------------ */

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Res<> entryPoint(u64 magic, Handover::Payload &payload) {
    return Hjert::Core::init(magic, payload);
}

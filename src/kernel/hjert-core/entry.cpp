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
    if (!Handover::valid(magic, payload)) {
        logInfo("entry: handover: invalid");
        return Error::invalidInput("Invalid handover payload");
    }

    logInfo("entry: handover: valid");
    logInfo("entry: handover: agent: '{}'", payload.agentName());

    usize totalFree = 0;
    logInfo("entry: dumpying handover records...");
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
    }

    logInfo("entry: handover: total free: {}mib", toMib(totalFree));

    return Ok();
}

Res<> enterUserspace(Handover::Payload &payload) {
    auto const *record = payload.fileByName("/servers/system");
    if (!record) {
        logInfo("entry: handover: no init file");
        return Error::invalidInput("No init file");
    }

    auto space = try$(Space::create());

    logInfo("entry: mapping elf...");
    auto elfMem = try$(VNode::makeDma(record->range<Hal::DmaRange>()));
    auto elfRange = try$(kmm().pmm2Kmm(elfMem->range()));
    Elf::Image image{elfRange.bytes()};

    if (!image.valid()) {
        logInfo("entry: invalid elf");
        return Error::invalidInput("Invalid elf");
    }

    for (auto prog : image.programs()) {
        logInfo("entry: program: {x} {x} {x} {x}", prog.offset(), prog.vaddr(), prog.memsz(), prog.filez());

        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);

        if (!!(prog.flags() & Elf::ProgramFlags::WRITE)) {
            auto sectionMem = try$(VNode::alloc(size, Hj::MemFlags::NONE));
            auto sectionRange = try$(kmm().pmm2Kmm(sectionMem->range()));
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(space->map({prog.vaddr(), size}, sectionMem, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(space->map({prog.vaddr(), size}, elfMem, prog.offset(), Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfo("entry: mapping stack...");
    auto STACK_SIZE = kib(16);
    auto stackMem = try$(VNode::alloc(STACK_SIZE, Hj::MemFlags::HIGH));
    auto stackRange = try$(space->map({}, stackMem, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("entry: creating task...");
    auto task = try$(Task::create(TaskType::USER, space));
    try$(Sched::self().start(task, image.header().entry, stackRange.end()));

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

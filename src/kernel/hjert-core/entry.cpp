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

Res<> validateAndDump(uint64_t magic, Handover::Payload &payload) {
    if (!Handover::valid(magic, payload)) {
        logInfo("entry: handover: invalid");
        return Error::invalidInput("Invalid handover payload");
    }

    logInfo("entry: handover: valid");
    logInfo("entry: handover: agent: '{}'", payload.agentName());

    size_t totalFree = 0;
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

void taskBody() {
    while (true) {
        Arch::loggerOut().writeRune('B').unwrap();
        Arch::cpu().relaxe();
    }
}

Res<> init(uint64_t magic, Handover::Payload &payload) {
    try$(Arch::init(payload));

    splash();
    try$(validateAndDump(magic, payload));

    try$(Mem::init(payload));
    try$(Sched::init(payload));

    auto taskB = try$(Task::create(try$(Space::create())));
    try$(Sched::self().start(taskB, (uintptr_t)taskBody));

    logInfo("entry: everything is ready, enabling interrupts...");
    Arch::cpu().retainEnable();
    Arch::cpu().enableInterrupts();

    while (true) {
        Arch::loggerOut().writeRune('A').unwrap();
        Arch::cpu().relaxe();
    }

    while (true)
        Arch::cpu().relaxe();
}

} // namespace Hjert::Core

/* --- Handover Entry Point ------ ------------------------------------------ */

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Res<> entryPoint(uint64_t magic, Handover::Payload &payload) {
    return Hjert::Core::init(magic, payload);
}

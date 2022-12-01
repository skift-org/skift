#include <handover/main.h>
#include <karm-base/size.h>
#include <karm-debug/logger.h>

#include "arch.h"
#include "cpu.h"
#include "mem.h"

namespace Hjert {

Error validateAndDump(uint64_t magic, Handover::Payload &payload) {
    if (!Handover::valid(magic, payload)) {
        Debug::linfo("handover: invalid");
        return "Invalid handover payload";
    }

    Debug::linfo("handover: valid");
    Debug::linfo("handover: agent: '{}'", payload.agentName());

    size_t totalFree = 0;
    for (auto const &record : payload) {
        Debug::linfo(
            "handover: record: {} {x}-{x} ({}kib)",
            record.name(),
            record.start,
            record.end(),
            record.size / 1024);

        if (record.tag == Handover::FREE) {
            totalFree += record.size;
        }
    }

    Debug::linfo("handover: total free: {}mib", toMib(totalFree));

    return OK;
}

Error start(uint64_t magic, Handover::Payload &payload) {
    try$(Arch::init(payload));

    Debug::linfo("hjert (v0.0.1)");
    try$(validateAndDump(magic, payload));

    Debug::linfo("Initialized memory manager...");
    try$(Mem::init(payload));

    Debug::linfo("Everything is ready, enabling interrupts...");
    Arch::cpu().retainEnable();
    Arch::cpu().enableInterrupts();

    while (true)
        Arch::cpu().relaxe();
}

} // namespace Hjert

/* --- Handover Entry Point ------ ------------------------------------------ */

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Error entryPoint(uint64_t magic, Handover::Payload &payload) {
    return Hjert::start(magic, payload);
}

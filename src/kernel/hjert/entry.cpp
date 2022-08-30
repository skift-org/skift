#include <handover/main.h>
#include <hjert/arch.h>
#include <karm-debug/logger.h>
#include <limine/main.h>

using namespace Hjert;

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Error entryPoint(uint64_t magic, Handover::Payload const &payload) {
    try$(Arch::init());

    if (!Handover::valid(magic, payload)) {
        return "Invalid handover payload";
    }

    Debug::linfo("hjert (v0.0.1)");

    for (auto const &record : payload) {
        Debug::linfo(
            "handover: entry: {} {x}-{x} ({}kib)",
            record.name(),
            record.start,
            record.end(), record.size / 1024);
    }

    Arch::stopCpu();
}

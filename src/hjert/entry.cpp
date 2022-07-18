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

    Arch::stopCpu();
}

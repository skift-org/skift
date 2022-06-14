#include <handover/main.h>
#include <hjert/arch.h>
#include <limine/main.h>

using namespace Hjert;

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Error entryPoint([[maybe_unused]] uint64_t magic, [[maybe_unused]] Handover::Payload const &payload) {
    try$(Arch::writeLog("hjert (v0.0.1)\n", 15));

    Arch::stopCpu();
}

#include <handover/main.h>
#include <hjert/arch.h>
#include <limine/main.h>

#include <arch-x86_64/gdt.h>

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

    try$(Arch::writeLog("hjert (v0.0.1)\n"));

    Arch::stopCpu();
}

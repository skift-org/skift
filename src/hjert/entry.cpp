#include <handover/main.h>
#include <hjert/arch.h>
#include <limine/main.h>

#include <arch-x86_64/gdt.h>

using namespace Hjert;

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Error entryPoint([[maybe_unused]] uint64_t magic, [[maybe_unused]] Handover::Payload const &payload) {
    try$(Arch::writeLog("hjert (v0.0.1)\n"));
    Arch::x86_64::gdtInitialize();

    Arch::stopCpu();
}

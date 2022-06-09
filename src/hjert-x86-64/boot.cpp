#include <handover/handover.h>
#include <handover/main.h>
#include <hjert/arch.h>

#include <arch-x86_64/asm.h>

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

void entryPoint([[maybe_unused]] uint64_t magic, [[maybe_unused]] Handover::Payload const &payload) {
    
    (void)Hjert::Arch::writeLog("hjert (v0.0.1)\n", 14);

    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

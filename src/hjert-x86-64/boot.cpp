#include <handover/handover.h>

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

void entryPoint([[maybe_unused]] uint64_t magic, [[maybe_unused]] Handover::Payload const &payload) {
}



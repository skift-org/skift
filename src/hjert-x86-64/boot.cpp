#include <handover/handover.h>

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

void entryPoint([[maybe_unused]] uint32_t magic, [[maybe_unused]] Handover::Payload const &payload) {
    for (auto record : payload){
        print("{}: {}-{}", record.tagName(), record.start, record.size);
    }

    payload.fileByName("init.elf");
}

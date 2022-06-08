#pragma once

#include "handover.h"

void entryPoint(uint64_t magic, Handover::Payload const &payload);

extern "C" void _kstart(uint64_t magic, Handover::Payload const *payload) {
    entryPoint(magic, *payload);
}

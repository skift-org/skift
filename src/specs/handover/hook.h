#pragma once

import Karm.Sys;

#include "spec.h"

struct HandoverHook : Sys::Service {
    Handover::Payload* payload;

    HandoverHook(Handover::Payload* payload)
        : payload(payload) {}
};

inline Handover::Payload& useHandover(Sys::Context& ctx = Sys::rootContext()) {
    return *ctx.use<HandoverHook>().payload;
}

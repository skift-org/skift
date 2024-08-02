#pragma once

#include <karm-sys/context.h>

#include "spec.h"

struct HandoverHook : public Sys::Service {
    Handover::Payload *payload;

    HandoverHook(Handover::Payload *payload)
        : payload(payload) {}
};

inline Handover::Payload &useHandover(Sys::Context &ctx = Sys::globalContext()) {
    return *ctx.use<HandoverHook>().payload;
}

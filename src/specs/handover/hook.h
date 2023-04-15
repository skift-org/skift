#pragma once

#include <karm-main/base.h>

#include "spec.h"

struct HandoverHook : public Hook {
    Handover::Payload *payload;

    HandoverHook(Handover::Payload *payload)
        : payload(payload) {}
};

inline Handover::Payload &useHandover(Ctx &ctx = Ctx::instance()) {
    return *ctx.use<HandoverHook>().payload;
}

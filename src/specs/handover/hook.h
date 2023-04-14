#pragma once

#include <karm-main/base.h>

#include "spec.h"

struct _HandoverHook : public Hook {
    Handover::Payload *payload;

    _HandoverHook(Handover::Payload *payload)
        : payload(payload) {}
};

inline Handover::Payload &useHandover(Ctx &ctx) {
    return *ctx.use<_HandoverHook>().payload;
}

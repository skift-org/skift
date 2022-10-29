#pragma once

#include <karm-base/error.h>

#include "spec.h"

Error entryPoint(uint64_t magic, Handover::Payload &payload);

#ifndef HANOVER_NO_KSTART

extern "C" void _kstart(uint64_t magic, Handover::Payload *payload) {
    Error err = entryPoint(magic, *payload);
    if (err) {
        panic(err.msg());
    }
}

#endif

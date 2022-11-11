#pragma once

#include <abi-sysv/abi.h>
#include <karm-base/error.h>

#include "spec.h"

Error entryPoint(uint64_t magic, Handover::Payload &payload);

#ifndef HANOVER_NO_KSTART

extern "C" void _kstart(uint64_t magic, Handover::Payload *payload) {
    Abi::SysV::init();
    Error err = entryPoint(magic, *payload);
    panic(err.msg());
}

#endif

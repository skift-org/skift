#pragma once

#include <skift/Time.h>

#include <libsystem/Common.h>
#include <libutils/SourceLocation.h>

#include "kernel/handover/Handover.h"

void system_main(Handover *handover);

void system_initialize();

void __no_return system_hang();

void __no_return system_stop();

void system_tick();

uint32_t system_get_tick();

ElapsedTime system_get_uptime();

#define system_panic(__args...) \
    system_panic_internal(::Utils::SourceLocation::current(), nullptr, __args)

#define system_panic_with_context(__context, __args...) \
    system_panic_internal(::Utils::SourceLocation::current(), __context, __args)

void __no_return system_panic_internal(
    Utils::SourceLocation location,
    void *stackframe,
    const char *message,
    ...);

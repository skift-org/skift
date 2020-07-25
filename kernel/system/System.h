#pragma once

#include <libsystem/Common.h>
#include <libsystem/Time.h>

#include "kernel/multiboot/Multiboot.h"

void system_main(Multiboot *multiboot);

void system_initialize(void);

void __no_return system_hang(void);

void __no_return system_stop(void);

void system_tick(void);

uint32_t system_get_tick(void);

ElapsedTime system_get_uptime(void);

#define system_panic(__args...) \
    system_panic_internal(SOURCE_LOCATION, nullptr, __args)

#define system_panic_with_context(__context, __args...) \
    system_panic_internal(SOURCE_LOCATION, __context, __args)

void __no_return system_panic_internal(
    __SOURCE_LOCATION__ location,
    void *stackframe,
    const char *message,
    ...);

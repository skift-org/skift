#pragma once

struct PlatformContext
{
    char fpu_registers[512];
};

PlatformContext *platform_context_create();

void platform_context_destroy(PlatformContext *context);

void platform_context_save(PlatformContext *context);

void platform_context_restore(PlatformContext *context);

#include <limine/spec.h>

#include <hal-x86_64/asm.h>

static auto volatile framebufferRequest =
    Limine::ask<Limine::Framebuffer>();

static void haltAndCatchFire() {
    x86_64::cli();
    for (;;)
        x86_64::hlt();
}

void _start(void) {
    if (not(framebufferRequest and framebufferRequest->framebuffer)) {
        haltAndCatchFire();
    }

    auto &framebuffer = framebufferRequest->framebuffer[0];

    auto *pixels = (u32 *)framebuffer.address;
    for (size_t i = 0; i < 100; i++) {
        pixels[i * (framebuffer.mode.pitch / 4) + i] = 0xffffff;
    }

    haltAndCatchFire();
}

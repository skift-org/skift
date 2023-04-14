#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

Res<> entryPoint(Ctx &ctx) {
    Hj::Task::self().label("system").unwrap();

    auto &handover = useHandover(ctx);

    auto *fb = handover.findTag(Handover::Tag::FB);
    Hj::Mem fbMem = Hj::createMem(Hj::ROOT, fb->start, fb->size, Hj::MemFlags::DMA).take();
    fbMem.label("framebuffer").unwrap();
    auto fbAddr = Hj::Space::self().map(fbMem, 0, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE).take();

    Gfx::MutPixels pixels = {(void *)fbAddr, {fb->fb.width, fb->fb.height}, fb->fb.pitch, Gfx::BGRA8888};
    pixels.clear(Gfx::ZINC800);
    pixels.clip(pixels.bound().shrink(10)).clear(Gfx::ZINC900);

    logInfo("Hello from system server!");
    return Ok();
}

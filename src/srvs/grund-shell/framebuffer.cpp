#include "framebuffer.h"

namespace Grund::Shell {

Res<Strong<Framebuffer>> Framebuffer::open(Sys::Context &ctx) {
    auto &handover = useHandover(ctx);
    auto *record = handover.findTag(Handover::Tag::FB);
    auto vmo = try$(Hj::Vmo::create(Hj::ROOT, record->start, record->size, Hj::VmoFlags::DMA));
    try$(vmo.label("framebuffer"));
    return Ok(makeStrong<Framebuffer>(*record, try$(Hj::map(vmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE))));
}

Framebuffer::Framebuffer(Handover::Record record, Hj::Mapped map)
    : _record(record), _map(std::move(map)) {
    logInfo(
        "fb: {x}-{x} {}x{}, {} stride",
        _map.range().start,
        _map.range().end(),
        _record.fb.width,
        _record.fb.height,
        _record.fb.pitch
    );
}

Karm::Gfx::MutPixels Framebuffer::mutPixels() {
    return {
        _map.mutBytes().buf(),
        {
            _record.fb.width,
            _record.fb.height,
        },
        _record.fb.pitch,
        Gfx::BGRA8888,
    };
}

} // namespace Grund::Shell

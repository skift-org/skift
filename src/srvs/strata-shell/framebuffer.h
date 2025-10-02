#pragma once

import Karm.Gfx;

#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-math/rect.h>

namespace Strata::Shell {

struct Framebuffer {
    static Res<Rc<Framebuffer>> open(Sys::Context& ctx)  {
        auto& handover = useHandover(ctx);
        auto* record = handover.findTag(Handover::Tag::FB);
        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, record->start, record->size, Hj::VmoFlags::DMA));
        try$(vmo.label("framebuffer"));
        return Ok(makeRc<Framebuffer>(*record, try$(Hj::map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}))));
    }

    Handover::Record _record;
    Hj::Mapped _map;

    Framebuffer(Handover::Record record, Hj::Mapped map)
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

    Gfx::MutPixels mutPixels() {
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

    Math::Recti bound() const {
        return {
            _record.fb.width,
            _record.fb.height,
        };
    }
};

} // namespace Strata::Shell

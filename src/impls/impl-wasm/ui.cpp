#include <karm-ui/_embed.h>

#include "externs.h"

namespace Karm::Ui::_Embed {

struct WasmHost :
    public Host {
    Media::Image _back;

    WasmHost(Child root, Media::Image back)
        : Host(root), _back(back) {
    }

    Gfx::MutPixels mutPixels() override {
        return _back;
    }

    Res<> wait(TimeStamp) override {
        return Error::notImplemented("wait");
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty)
            embedFlipCanvas((uintptr_t)_back.pixels().clip(d)._buf);
    }

    void bubble(Sys::Event &e) override {
        Host::bubble(e);
    }
};

Res<Strong<Host>> makeHost(Child root) {
    auto back = Media::Image::alloc({(long)embedGetCanvasWidth(), (long)embedGetCanvasHeight()}, Gfx::BGRA8888);
    return Ok(makeStrong<WasmHost>(root, back));
}
} // namespace Karm::Ui::_Embed
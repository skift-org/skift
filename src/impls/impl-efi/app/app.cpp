module;

#include <efi/base.h>
#include <karm-core/macros.h>

module Karm.App;

import Karm.Core;
import Karm.Gfx;
import Karm.Sys;

namespace Karm::App::_Embed {

struct EfiApplication;

struct EfiWindow : Window {
    Gfx::MutPixels _front;
    Rc<Gfx::Surface> _back;

    EfiWindow(Gfx::MutPixels front, Rc<Gfx::Surface> back)
        : _front(front), _back(back) {}

    WindowId id() override {
        return WindowId{0};
    }

    Math::Recti bound() override {
        return _front.bound();
    }

    f64 scaleFactor() override {
        return 1.0;
    }

    Gfx::MutPixels acquireSurface() override {
        return _back->mutPixels();
    }

    void releaseSurface() override {
        Gfx::blitUnsafe(_front, _back->pixels());
    }

    void drag(DragEvent) override {
        // NO-OP
    }
};

struct EfiApplication : Application {
    Efi::SimpleTextInputProtocol* _stip = nullptr;

    explicit EfiApplication(Efi::SimpleTextInputProtocol* stip)
        : _stip(stip) {}

    Async::Task<Rc<Window>> createWindowAsync(WindowProps const&, Async::CancellationToken) override {
        auto* gop = co_try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
        auto* mode = gop->mode;

        Gfx::MutPixels front = {
            (void*)mode->frameBufferBase,
            {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
            (u16)(mode->info->pixelsPerScanLine * 4),
            Gfx::BGRA8888,
        };

        auto back = Gfx::Surface::alloc({front.width(), front.height()}, Gfx::BGRA8888);

        co_return Ok(makeRc<EfiWindow>(front, back));
    }

    Async::Task<> runAsync(Rc<Handler> handler, Async::CancellationToken ct) override {
        Duration const frameDuration = Duration::fromMSecs(16);
        Instant nextFrameTime = Sys::instant();

        while (not ct.cancelled()) {
            nextFrameTime = nextFrameTime + frameDuration;

            Efi::Key key = {};
            Res<> status = _stip->readKeyStroke(_stip, &key);
            if (status) {
                auto e = key.toKeyEvent();
                handler->handle<App::KeyboardEvent>(WindowId{0}, e);
            }

            handler->update();

            co_trya$(Sys::globalSched().sleepAsync(nextFrameTime, ct));

            Instant now = Sys::instant();
            if (now > nextFrameTime + frameDuration) {
                nextFrameTime = now;
            }
        }

        co_return Ok();
    }
};

Async::Task<Rc<Application>> createAppAsync(Sys::Context&, ApplicationProps const&, Async::CancellationToken) {
    auto* stip = co_try$(Efi::locateProtocol<Efi::SimpleTextInputProtocol>());
    co_return Ok(makeRc<EfiApplication>(stip));
}

} // namespace Karm::App::_Embed
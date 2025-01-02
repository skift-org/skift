#include <hideo-shell/app.h>
#include <karm-app/host.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-image/loader.h>
#include <karm-rpc/base.h>
#include <karm-sys/entry.h>
#include <karm-sys/time.h>
#include <karm-ui/host.h>
#include <mdi/calculator.h>

#include "../grund-bus/api.h"
#include "framebuffer.h"
#include "input.h"

namespace Grund::Shell {

struct Root : public Ui::ProxyNode<Root> {
    Vec<Math::Recti> _dirty;
    Strong<Gfx::CpuSurface> _frontbuffer;
    Strong<Gfx::Surface> _backbuffer;
    bool _shouldLayout{};

    Root(Ui::Child child, Strong<Gfx::CpuSurface> frontbuffer)
        : Ui::ProxyNode<Root>(std::move(child)),
          _frontbuffer(std::move(frontbuffer)),
          _backbuffer(Gfx::Surface::alloc(_frontbuffer->bound().size(), Gfx::BGRA8888)) {
    }

    void _repaint() {
        Gfx::CpuCanvas g;
        g.begin(*_backbuffer);
        for (auto &r : _dirty) {
            g.push();
            g.clear(r, Ui::GRAY950);
            g.fillStyle(Ui::GRAY50);
            g.clip(r.cast<f64>());
            paint(g, r);
            g.pop();

            Gfx::blitUnsafe(_frontbuffer->mutPixels(), _backbuffer->pixels());
        }
        g.end();

        _dirty.clear();
    }

    Async::Task<> run() {
        _shouldLayout = true;

        TimeStamp lastFrame = Sys::now();
        while (true) {
            auto frameStart = Sys::now();
            while (lastFrame < frameStart) {
                auto e = App::makeEvent<Node::AnimateEvent>(Ui::FRAME_TIME);
                event(*e);
                lastFrame += 16_ms;
            }

            if (_shouldLayout) {
                layout(bound());
                _shouldLayout = false;
                _dirty.clear();
                _dirty.pushBack(_backbuffer->bound());
            }

            if (_dirty.len() > 0) {
                _repaint();
            }

            co_trya$(Sys::globalSched().sleepAsync(lastFrame + 16_ms));
        }
    }

    Math::Recti bound() override {
        return _backbuffer->bound();
    }

    void _damage(Math::Recti r) {
        for (auto &d : _dirty) {
            if (d.colide(r)) {
                d = d.mergeWith(r);
                return;
            }
        }

        _dirty.pushBack(r);
    }

    void bubble(App::Event &event) override {
        if (auto e = event.is<Node::PaintEvent>()) {
            _damage(e->bound);
            event.accept();
        } else if (auto e = event.is<Node::LayoutEvent>()) {
            _shouldLayout = true;
            event.accept();
        } else if (auto e = event.is<Node::AnimateEvent>()) {
            event.accept();
        } else if (auto e = event.is<App::RequestExitEvent>()) {
            event.accept();
        }

        if (not event.accepted()) {
            logWarn("unhandled event, bouncing down");
            this->event(event);
        }
    }
};

Async::Task<> servAsync(Sys::Context &ctx) {
    Hideo::Shell::State state = {
        .isMobile = false,
        .dateTime = Sys::dateTime(),
        .background = co_try$(Image::loadOrFallback("bundle://hideo-shell/wallpapers/winter.qoi"_url)),
        .noti = {},
        .manifests = {
            makeStrong<Hideo::Shell::Manifest>(Mdi::CALCULATOR, "hideo-calculator.main"s, Gfx::ORANGE_RAMP),
        },
        .instances = {}
    };

    auto endpoint = Rpc::Endpoint::create(ctx);
    auto root = makeStrong<Root>(
        Hideo::Shell::app(std::move(state)) | inputTranslator,
        co_try$(Framebuffer::open(ctx))
    );

    Async::detach(root->run());

    co_try$(endpoint.send<Grund::Bus::Api::Listen>(Rpc::Port::BUS, Meta::idOf<App::MouseEvent>()));
    co_try$(endpoint.send<Grund::Bus::Api::Listen>(Rpc::Port::BUS, Meta::idOf<App::KeyboardEvent>()));

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());

        if (msg.is<App::MouseEvent>()) {
            auto rawEvent = msg.unpack<App::MouseEvent>().unwrap();
            auto event = App::makeEvent<App::MouseEvent>(rawEvent);
            root->child().event(*event);
        } else if (msg.is<App::KeyboardEvent>()) {
            auto event = msg.unpack<App::MouseEvent>();
        } else {
            logWarn("unsupported event: {}", msg.header());
        }
    }
}

} // namespace Grund::Shell

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Shell::servAsync(ctx);
}

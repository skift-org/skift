#include <karm-gfx/cpu/canvas.h>
#include <karm-gfx/icon.h>
#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>
#include <karm-sys/time.h>

#include "../strata-bus/api.h"
#include "api.h"
#include "framebuffer.h"
#include "input.h"

import Mdi;
import Karm.Ui;
import Karm.App;
import Hideo.Shell;
import Karm.Image;

namespace Strata::Shell {

struct Root : public Ui::ProxyNode<Root> {
    Vec<Math::Recti> _dirty;
    Rc<Framebuffer> _frontbuffer;
    Rc<Gfx::Surface> _backbuffer;
    bool _shouldLayout{};

    Root(Ui::Child child, Rc<Framebuffer> frontbuffer)
        : Ui::ProxyNode<Root>(std::move(child)),
          _frontbuffer(std::move(frontbuffer)),
          _backbuffer(Gfx::Surface::alloc(_frontbuffer->bound().size(), Gfx::BGRA8888)) {
    }

    void _repaint() {
        Gfx::CpuCanvas g;
        g.begin(*_backbuffer);
        for (auto& r : _dirty) {
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

        auto lastFrame = Sys::instant();
        while (true) {
            auto frameStart = Sys::instant();
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
        for (auto& d : _dirty) {
            if (d.colide(r)) {
                d = d.mergeWith(r);
                return;
            }
        }

        _dirty.pushBack(r);
    }

    void bubble(App::Event& event) override {
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

struct ServiceInstance : public Hideo::Shell::Instance {
    Ui::Child build() const override {
        return Ui::empty() | Ui::box({
                                 .backgroundFill = Ui::GRAY950,
                             });
    }
};

struct ServiceLauncher : public Hideo::Shell::Launcher {
    String componentId;

    ServiceLauncher(Gfx::Icon icon, String name, Gfx::ColorRamp ramp, String serviceId)
        : Launcher(icon, name, ramp), componentId(serviceId) {}

    void launch(Hideo::Shell::State&) override {
        Sys::globalEndpoint().send<Bus::Api::Start>(Sys::Port::BUS, componentId).unwrap();
    }
};

Async::Task<> servAsync(Sys::Context& ctx) {
    Hideo::Shell::State state = {
        .isMobile = false,
        .dateTime = Sys::dateTime(),
        .background = co_try$(Image::loadOrFallback("bundle://hideo-shell/wallpapers/winter.qoi"_url)),
        .noti = {},
        .launchers = {
            makeRc<Hideo::Shell::MockLauncher>(Mdi::CALCULATOR, "Calculator (Mocked)"s, Gfx::ORANGE_RAMP),
            makeRc<Strata::Shell::ServiceLauncher>(Mdi::CALCULATOR, "Calculator (Real)"s, Gfx::ORANGE_RAMP, "hideo-calculator.main"s),
        },
        .instances = {}
    };

    auto endpoint = Sys::Endpoint::create(ctx);
    auto root = makeRc<Root>(
        Hideo::Shell::app(std::move(state)) | inputTranslator,
        co_try$(Framebuffer::open(ctx))
    );

    Async::detach(root->run());

    co_try$(endpoint.send<Strata::Bus::Api::Listen>(Sys::Port::BUS, Meta::idOf<App::MouseEvent>()));
    co_try$(endpoint.send<Strata::Bus::Api::Listen>(Sys::Port::BUS, Meta::idOf<App::KeyboardEvent>()));

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());

        if (msg.is<App::MouseEvent>()) {
            auto rawEvent = msg.unpack<App::MouseEvent>().unwrap();
            auto event = App::makeEvent<App::MouseEvent>(rawEvent);
            root->child().event(*event);
        } else if (msg.is<App::KeyboardEvent>()) {
            auto event = msg.unpack<App::MouseEvent>();
        } else if (msg.is<Api::CreateInstance>()) {
            auto call = msg.unpack<Api::CreateInstance>().unwrap();
            logDebug("create instance {}", call.size);
            auto instance = makeRc<ServiceInstance>();
            instance->bound = {100, call.size};
            Hideo::Shell::Model::event(*root, Hideo::Shell::AddInstance{instance});
            (void)msg.packResp<Api::CreateInstance>(0uz);
        } else {
            logWarn("unsupported event: {}", msg.header());
        }
    }
}

} // namespace Strata::Shell

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Shell::servAsync(ctx);
}

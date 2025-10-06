module;

#include <karm-core/macros.h>

module Karm.Ui;

import Strata.Protos;
import Karm.Sys;
import Karm.App;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
import Karm.Core;

namespace Karm::Ui::_Embed {

struct Host : ProxyNode<Host> {
    Sys::Endpoint& _endpoint;

    Sys::Port _shell;
    Strata::IShell::WindowId _window;

    Rc<Strata::Protos::Surface> _surface;

    Vec<Math::Recti> _dirty;
    bool _shouldLayout{};

    Host(Child child, Sys::Endpoint& endpoint, Sys::Port shell, Strata::IShell::WindowId window, Rc<Strata::Protos::Surface> surface)
        : Ui::ProxyNode<Host>(std::move(child)), _endpoint(endpoint), _shell(shell), _window(window), _surface(surface) {}

    Math::Recti bound() override {
        return _surface->pixels().bound();
    }

    Async::Task<> _repaintAsync() {
        Gfx::CpuCanvas g;
        g.begin(_surface->mutPixels());

        Math::Recti damaged = _dirty[0];
        for (auto& r : _dirty) {
            g.push();
            g.clear(r, Ui::GRAY950);
            g.fillStyle(Ui::GRAY50);
            g.clip(r.cast<f64>());
            paint(g, r);
            g.pop();

            damaged = damaged.mergeWith(r);
        }
        g.end();

        co_trya$(_endpoint.callAsync<Strata::IShell::WindowFlip>(_shell, _window, damaged));

        _dirty.clear();

        co_return Ok();
    }

    Async::Task<> runAsync() {
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
                _dirty.pushBack(_surface->pixels().bound());
            }

            if (_dirty.len() > 0) {
                co_trya$(_repaintAsync());
            }

            co_trya$(Sys::globalSched().sleepAsync(lastFrame + 16_ms));
        }
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
        } else if (auto e = event.is<Ui::DragEvent>()) {
            if (e->type == Ui::DragEvent::START)
                (void)_endpoint.send<Strata::IShell::WindowMove>(_shell, _window);
        } else if (auto e = event.is<App::RequestExitEvent>()) {
            (void)_endpoint.send<Strata::IShell::WindowDestroy>(_shell, _window);
            logInfo("application closed");
            Sys::exit(Ok());
        }

        if (not event.accepted()) {
            logWarn("unhandled event, bouncing down");
            this->event(event);
        }
    }
};

Async::Task<> runAsync(Sys::Context& ctx, Child root) {
    auto endpoint = Sys::Endpoint::adopt(ctx);
    auto shell = co_trya$(endpoint.callAsync<Strata::IBus::Locate>(Sys::Port::BUS, "strata-shell"s));
    auto size = root->size({1024, 720}, Hint::MIN);

    auto [window, actual] = co_trya$(endpoint.callAsync<Strata::IShell::WindowCreate>(shell, size));

    auto surface = co_try$(Strata::Protos::Surface::create(actual.size));
    co_try$(endpoint.send<Strata::IShell::WindowAttach>(shell, window, surface));

    auto host = makeRc<Host>(std::move(root), endpoint, shell, window, surface);
    Async::detach(host->runAsync());

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());
        if (msg.is<Strata::IShell::WindowEvent>()) {
            auto m = co_try$(msg.unpack<Strata::IShell::WindowEvent>());
            auto event = App::makeEvent<App::MouseEvent>(m.event.unwrap<App::MouseEvent>());
            host->event(event);
        } else {
            logWarn("unsupported event: {}", msg.header());
        }
    }
}

} // namespace Karm::Ui::_Embed

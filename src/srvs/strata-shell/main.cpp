#include <karm-sys/entry.h>

#include "framebuffer.h"
#include "hideo-spreadsheet/model.h"
#include "input.h"

import Mdi;
import Karm.Ui;
import Karm.App;
import Karm.Gfx;
import Hideo.Shell;
import Karm.Image;
import Strata.Protos;

namespace Strata::Shell {

struct Root : Ui::ProxyNode<Root> {
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
        }
        g.end();

        Gfx::blitUnsafe(_frontbuffer->mutPixels(), _backbuffer->pixels());
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

            // when exiting lastFrame may be 16ms in the future
            // clamp it to now to avoid a long sleep
            lastFrame = frameStart;

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

struct ServiceInstance : Hideo::Shell::Window {
    Sys::Endpoint& _endpoint;
    Sys::Port _client;
    IShell::WindowId _windowId;
    Rc<Gfx::Surface> _frontbuffer;
    Opt<Rc<Protos::Surface>> _backbuffer;

    ServiceInstance(Sys::Endpoint& endpoint, Sys::Port client, Rc<Gfx::Surface> frontbuffer)
        : _endpoint(endpoint), _client(client), _frontbuffer(frontbuffer) {
    }

    Rc<Gfx::Surface> surface() const override {
        return _frontbuffer;
    }

    void event(App::Event& e) override {
        if (auto it = e.is<App::MouseEvent>())
            (void)_endpoint.send<IShell::WindowEvent>(_client, _windowId, *it);
    }


    void attach(Rc<Protos::Surface> backbuffer) {
        _backbuffer = backbuffer;
        Gfx::blitUnsafe(_frontbuffer->mutPixels(), _backbuffer.unwrap()->pixels());
    }
};

struct ServiceLauncher : Hideo::Shell::Launcher {
    String componentId;

    ServiceLauncher(Gfx::Icon icon, String name, Gfx::ColorRamp ramp, String serviceId)
        : Launcher(icon, name, ramp), componentId(serviceId) {}

    void launch(Hideo::Shell::State&) override {
        Sys::globalEndpoint().send<IBus::Start>(Sys::Port::BUS, componentId).unwrap();
    }
};

Async::Task<> servAsync(Sys::Context& ctx) {
    Hideo::Shell::State state = {
        .dateTime = Sys::dateTime(),
        .background = co_try$(Image::loadOrFallback("bundle://hideo-shell/wallpapers/abstract.qoi"_url)),
        .noti = {},
        .launchers = {
            makeRc<ServiceLauncher>(Mdi::INFORMATION_OUTLINE, "About"s, Gfx::BLUE_RAMP, "hideo-about.main"s),
            makeRc<ServiceLauncher>(Mdi::WEB, "Browser"s, Gfx::BLUE_RAMP, "vaev-browser.main"s),
            makeRc<ServiceLauncher>(Mdi::CALCULATOR, "Calculator"s, Gfx::ORANGE_RAMP, "hideo-calculator.main"s),
            makeRc<ServiceLauncher>(Mdi::MESSAGE, "Chat"s, Gfx::GREEN_RAMP, "hideo-chat.main"s),
            makeRc<ServiceLauncher>(Mdi::CONSOLE_LINE, "Console"s, Gfx::SLATE_RAMP, "hideo-console.main"s),
            makeRc<ServiceLauncher>(Mdi::CLOCK, "Clock"s, Gfx::INDIGO_RAMP, "hideo-clock.main"s),
            makeRc<ServiceLauncher>(Mdi::COUNTER, "Counter"s, Gfx::GREEN_RAMP, "hideo-counter.main"s),
            makeRc<ServiceLauncher>(Mdi::FOLDER, "Files"s, Gfx::AMBER_RAMP, "hideo-files.main"s),
            makeRc<ServiceLauncher>(Mdi::COG, "Settings"s, Gfx::BLUE_RAMP, "hideo-settings.main"s),
            makeRc<ServiceLauncher>(Mdi::VIEW_DASHBOARD, "Sysmon"s, Gfx::RED_RAMP, "hideo-sysmon.main"s),
            makeRc<ServiceLauncher>(Mdi::PEN, "Text"s, Gfx::BLUE_RAMP, "hideo-text.main"s),
            makeRc<ServiceLauncher>(Mdi::DUCK, "Zoo"s, Gfx::TEAL_RAMP, "hideo-zoo.main"s),
        },
        .instances = {}
    };

    auto endpoint = Sys::Endpoint::adopt(ctx);
    auto framebuffer = co_try$(Framebuffer::open(ctx));
    auto app = Hideo::Shell::app(std::move(state));

    auto root = makeRc<Root>(
        inputTranslator(app, framebuffer->bound().center()),
        framebuffer
    );

    Async::detach(root->run());

    co_try$(endpoint.send<Strata::IBus::Listen>(Sys::Port::BUS, Meta::idOf<App::MouseEvent>()));
    co_try$(endpoint.send<Strata::IBus::Listen>(Sys::Port::BUS, Meta::idOf<App::KeyboardEvent>()));

    IShell::WindowId windowIdAllocator = 1;
    Map<IShell::WindowId, Rc<ServiceInstance>> windows;

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());

        if (msg.is<App::MouseEvent>()) {
            auto rawEvent = msg.unpack<App::MouseEvent>().unwrap();
            auto event = App::makeEvent<App::MouseEvent>(rawEvent);
            root->child().event(*event);
        }

        else if (msg.is<App::KeyboardEvent>()) {
            auto event = msg.unpack<App::KeyboardEvent>();
        }

        else if (msg.is<IShell::WindowCreate>()) {
            auto call = msg.unpack<IShell::WindowCreate>().unwrap();
            logDebug("window create {}", call.want.size);

            auto id = windowIdAllocator++;
            auto frontbuffer = Gfx::Surface::alloc(call.want.size);
            frontbuffer->mutPixels().clear(Ui::GRAY950);
            auto instance = makeRc<ServiceInstance>(endpoint, msg.header().from, frontbuffer);
            instance->bound = Math::Recti{call.want.size}.center(framebuffer->bound());
            windows.put(id, instance);
            Hideo::Shell::Model::event(*root, Hideo::Shell::AddInstance{instance});

            call.want.formFactor = App::formFactor;
            (void)endpoint.resp<IShell::WindowCreate>(msg, Ok<IShell::WindowCreate::Response>(id, call.want));
        }

        else if (msg.is<IShell::WindowDestroy>()) {
            auto call = msg.unpack<IShell::WindowDestroy>().unwrap();

            auto instance = windows.get(call.window);
            Hideo::Shell::Model::event(*root, Hideo::Shell::RemoveInstance{instance});
            windows.del(call.window);

            (void)endpoint.resp<IShell::WindowDestroy>(msg, Ok());
        }

        else if (msg.is<IShell::WindowAttach>()) {
            auto call = msg.unpack<IShell::WindowAttach>().unwrap();

            auto instance = windows.get(call.window);
            instance->attach(call.buffer.unwrap());
            Ui::shouldRepaint(*root);
        }

        else if (msg.is<IShell::WindowFlip>()) {
            auto call = msg.unpack<IShell::WindowFlip>().unwrap();

            auto instance = windows.get(call.window);
            Gfx::blitUnsafe(instance->_frontbuffer->mutPixels(), instance->_backbuffer.unwrap()->pixels());
            Ui::shouldRepaint(*root);

            (void)endpoint.resp<IShell::WindowFlip>(msg, Ok());
        }

        else if (msg.is<IShell::WindowMove>()) {
            auto call = msg.unpack<IShell::WindowMove>().unwrap();

            auto instance = windows.get(call.window);
            instance->dragged = true;
        }

        else {
            logWarn("unsupported event: {}", msg.header());
        }
    }
}

} // namespace Strata::Shell

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Shell::servAsync(ctx);
}

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

    Async::Task<> runAsync(Async::CancellationToken ct) {
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

            co_trya$(Sys::globalSched().sleepAsync(lastFrame + 16_ms, ct));
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
        } else if (auto e = event.is<App::RequestCloseEvent>()) {
            event.accept();
        }

        if (not event.accepted()) {
            logWarn("unhandled event, bouncing down");
            this->event(event);
        }
    }
};

struct ClientWindow : Hideo::Shell::Window {
    Sys::Endpoint& _endpoint;
    Sys::Port _client;
    IShell::WindowId _id;
    Rc<Gfx::Surface> _frontbuffer;
    Opt<Rc<Protos::Surface>> _backbuffer;

    ClientWindow(Sys::Endpoint& endpoint, Sys::Port client, Rc<Gfx::Surface> frontbuffer)
        : _endpoint(endpoint), _client(client), _frontbuffer(frontbuffer) {
    }

    Rc<Gfx::Surface> surface() const override {
        return _frontbuffer;
    }

    void event(App::Event& e) override {
        if (auto it = e.is<App::MouseEvent>()) {
            (void)_endpoint.send(_client, IShell::WindowEvent{_id, *it});
            e.accept();
        } else if (auto it = e.is<App::KeyboardEvent>()) {
            (void)_endpoint.send(_client, IShell::WindowEvent{_id, *it});
            e.accept();
        } else if (auto it = e.is<App::TypeEvent>()) {
            (void)_endpoint.send(_client, IShell::WindowEvent{_id, *it});
            e.accept();
        }
    }

    void resize(App::Snap snap, Math::Vec2i size) override {
        if (size != activeBound().wh) {
            logDebug("resize: {}", size);
            _frontbuffer = Gfx::Surface::alloc(size);
            (void)_endpoint.send(
                _client,
                IShell::WindowUpdate{
                    _id,
                    {
                        .size = size,
                        .formFactor = App::formFactor,
                    },
                }
            );
        }
        Window::resize(snap, size);
    }

    void attach(Rc<Protos::Surface> backbuffer) {
        _backbuffer = backbuffer;
        flip();
    }

    void flip() {
        // FIXME: Only blit the region that was updated
        auto region = _frontbuffer->mutPixels().bound().clipTo(_backbuffer.unwrap()->pixels().bound());
        Gfx::blitUnsafe(_frontbuffer->mutPixels().clip(region), _backbuffer.unwrap()->pixels().clip(region));
    }
};

struct ComponentLauncher : Hideo::Shell::Launcher {
    String componentId;

    ComponentLauncher(Gfx::Icon icon, String name, Gfx::ColorRamp ramp, String componentId)
        : Launcher(icon, name, ramp), componentId(componentId) {}

    void launch(Hideo::Shell::State&) override {
        Sys::globalEndpoint().send(Sys::Port::BUS, IBus::Start{componentId}).unwrap();
    }
};

struct Server {
    Sys::Endpoint& _endpoint;

    Rc<Root> _root;
    Rc<Framebuffer> _framebuffer;

    IShell::WindowId _windowId = 1;
    Map<IShell::WindowId, Rc<ClientWindow>> _windows = {};

    static Res<Server> create(Sys::Context& ctx, Sys::Endpoint& endpoint) {
        Hideo::Shell::State state = {
            .dateTime = Sys::dateTime(),
            .background = try$(Image::loadOrFallback("bundle://hideo-shell/wallpapers/abstract.qoi"_url)),
            .noti = {},
            .launchers = {
                makeRc<ComponentLauncher>(Mdi::INFORMATION_OUTLINE, "About"s, Gfx::BLUE_RAMP, "hideo-about.main"s),
                makeRc<ComponentLauncher>(Mdi::WEB, "Browser"s, Gfx::BLUE_RAMP, "vaev-browser.main"s),
                makeRc<ComponentLauncher>(Mdi::CALCULATOR, "Calculator"s, Gfx::ORANGE_RAMP, "hideo-calculator.main"s),
                makeRc<ComponentLauncher>(Mdi::MESSAGE, "Chat"s, Gfx::GREEN_RAMP, "hideo-chat.main"s),
                makeRc<ComponentLauncher>(Mdi::CONSOLE_LINE, "Console"s, Gfx::SLATE_RAMP, "hideo-console.main"s),
                makeRc<ComponentLauncher>(Mdi::CLOCK, "Clock"s, Gfx::INDIGO_RAMP, "hideo-clock.main"s),
                makeRc<ComponentLauncher>(Mdi::COUNTER, "Counter"s, Gfx::GREEN_RAMP, "hideo-counter.main"s),
                makeRc<ComponentLauncher>(Mdi::FOLDER, "Files"s, Gfx::AMBER_RAMP, "hideo-files.main"s),
                makeRc<ComponentLauncher>(Mdi::COG, "Settings"s, Gfx::BLUE_RAMP, "hideo-settings.main"s),
                makeRc<ComponentLauncher>(Mdi::VIEW_DASHBOARD, "Sysmon"s, Gfx::RED_RAMP, "hideo-sysmon.main"s),
                makeRc<ComponentLauncher>(Mdi::PEN, "Text"s, Gfx::BLUE_RAMP, "hideo-text.main"s),
                makeRc<ComponentLauncher>(Mdi::DUCK, "Zoo"s, Gfx::TEAL_RAMP, "hideo-zoo.main"s),
            },
            .windows = {}
        };

        auto framebuffer = try$(Framebuffer::open(ctx));
        auto app = Hideo::Shell::app(std::move(state));

        auto root = makeRc<Root>(
            inputTranslator(app, framebuffer->bound().center()),
            framebuffer
        );

        try$(endpoint.send(
            Sys::Port::BUS,
            Strata::IBus::Listen{
                Meta::idOf<App::MouseEvent>(),
            }
        ));
        try$(endpoint.send(
            Sys::Port::BUS,
            Strata::IBus::Listen{
                Meta::idOf<App::KeyboardEvent>()
            }
        ));
        try$(endpoint.send(
            Sys::Port::BUS,
            Strata::IBus::Listen{
                Meta::idOf<App::TypeEvent>()
            }
        ));

        return Ok(Server{
            endpoint,
            root,
            framebuffer,
        });
    }

    Res<> _handleMouseEvent(Sys::Message& message) {
        auto event = try$(message.unpack<App::MouseEvent>());
        auto e = App::makeEvent<App::MouseEvent>(event);
        _root->child().event(*e);
        return Ok();
    }

    Res<> _handleKeyboardEvent(Sys::Message& message) {
        auto event = try$(message.unpack<App::KeyboardEvent>());
        auto e = App::makeEvent<App::KeyboardEvent>(event);
        _root->child().event(*e);
        return Ok();
    }

    Res<> _handleTypeEvent(Sys::Message& message) {
        auto event = try$(message.unpack<App::TypeEvent>());
        auto e = App::makeEvent<App::TypeEvent>(event);
        _root->child().event(*e);
        return Ok();
    }

    Res<IShell::WindowCreate::Response> _handleWindowCreate(Sys::Message& message) {
        auto [want] = try$(message.unpack<IShell::WindowCreate>());
        logDebug("window create {}", want.size);

        auto windowId = _windowId++;

        auto windowSurface = Gfx::Surface::alloc(want.size);
        windowSurface->mutPixels().clear(Ui::GRAY950);

        auto window = makeRc<ClientWindow>(_endpoint, message.header().from, windowSurface);
        window->_floatingBound = Math::Recti{want.size}.center(_framebuffer->bound());
        window->_id = windowId;
        _windows.put(windowId, window);
        Hideo::Shell::Model::event(*_root, Hideo::Shell::AddWindow{window});

        auto offer = want;
        offer.formFactor = App::formFactor;
        return Ok(IShell::WindowCreate::Response{
            windowId,
            offer,
        });
    }

    Res<IShell::WindowDestroy::Response> _handleWindowDestroy(Sys::Message& message) {
        auto [windowId] = try$(message.unpack<IShell::WindowDestroy>());

        auto window = _windows.get(windowId);
        Hideo::Shell::Model::event(*_root, Hideo::Shell::RemoveWindow{window});
        _windows.del(windowId);

        return Ok();
    }

    Res<IShell::WindowAttach::Response> _handleWindowAttach(Sys::Message& message) {
        auto [windowId, buffer] = try$(message.unpack<IShell::WindowAttach>());

        auto window = _windows.get(windowId);
        window->attach(buffer.unwrap());
        Ui::shouldRepaint(*_root);

        return Ok();
    }

    Res<> _handleWindowFlip(Sys::Message& message) {
        auto [windowId, region] = try$(message.unpack<IShell::WindowFlip>());

        auto window = _windows.get(windowId);
        window->flip();
        Ui::shouldRepaint(*_root);

        return Ok();
    }

    Res<> _handleWindowMove(Sys::Message& message) {
        auto [windowId] = try$(message.unpack<IShell::WindowMove>());
        auto window = _windows.get(windowId);
        window->dragged = true;
        return Ok();
    }

    Res<> _handleWindowSnap(Sys::Message& message) {
        auto [windowId, snap] = try$(message.unpack<IShell::WindowSnap>());
        auto window = _windows.get(windowId);
        Hideo::Shell::Model::event(*_root, Hideo::Shell::SnapWindow{window, snap});
        return Ok();
    }

    Async::Task<> servAsync(Async::CancellationToken ct) {
        Async::detach(_root->runAsync(ct));
        Map<IShell::WindowId, Rc<ClientWindow>> windows;
        while (true) {
            auto msg = co_trya$(_endpoint.recvAsync(ct));
            if (msg.is<App::MouseEvent>())
                (void)_handleMouseEvent(msg);
            else if (msg.is<App::KeyboardEvent>())
                (void)_handleKeyboardEvent(msg);
            else if (msg.is<App::TypeEvent>())
                (void)_handleTypeEvent(msg);
            else if (msg.is<IShell::WindowCreate>())
                (void)_endpoint.resp<IShell::WindowCreate>(msg, _handleWindowCreate(msg));
            else if (msg.is<IShell::WindowDestroy>())
                (void)_endpoint.resp<IShell::WindowDestroy>(msg, _handleWindowDestroy(msg));
            else if (msg.is<IShell::WindowAttach>())
                (void)_endpoint.resp<IShell::WindowAttach>(msg, _handleWindowAttach(msg));
            else if (msg.is<IShell::WindowFlip>())
                (void)_endpoint.resp<IShell::WindowFlip>(msg, _handleWindowFlip(msg));
            else if (msg.is<IShell::WindowMove>())
                (void)_handleWindowMove(msg);
            else if (msg.is<IShell::WindowSnap>())
                (void)_handleWindowSnap(msg);
            else
                logWarn("unsupported message: {}", msg.header());
        }
    }
};

} // namespace Strata::Shell

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto endpoint = Sys::Endpoint::adopt(ctx);
    auto server = co_try$(Strata::Shell::Server::create(ctx, endpoint));
    co_return co_await server.servAsync(ct);
}

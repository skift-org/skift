module;

#include <karm-core/macros.h>

module Karm.App;

import Karm.Core;
import Karm.Gfx;
import Karm.Sys;
import Strata.Protos;

namespace Karm::App::_Embed {

struct SkiftApplication;

struct SkiftWindow : Window {
    SkiftApplication& _application;
    Strata::IShell::WindowId _id;
    Rc<Strata::Protos::Surface> _surface;
    bool _dirty = false;
    bool _closed = false;

    SkiftWindow(SkiftApplication& application, Strata::IShell::WindowId id, Rc<Strata::Protos::Surface> surface)
        : _application(application), _id(id), _surface(surface) {}

    ~SkiftWindow();

    WindowId id() override {
        return WindowId{_id};
    }

    Math::Recti bound() override {
        return _surface->pixels().bound();
    }

    f64 scaleFactor() override {
        return 1.0;
    }

    Gfx::MutPixels acquireSurface() override {
        _dirty = true;
        return _surface->mutPixels();
    }

    void releaseSurface(Slice<Math::Recti>) override;

    void drag(DragEvent) override;

    void resize(Direction) override {}

    void maximize() override {}

    void minimize() override {}

    void close() override {
        _closed = true;
    }
};

struct SkiftApplication : Application {
    Rc<Sys::Endpoint> _endpoint;
    Sys::Port _shell;
    Map<usize, SkiftWindow*> _windows;
    bool _exited = false;

    explicit SkiftApplication(Rc<Sys::Endpoint> endpoint, Sys::Port shell)
        : _endpoint(endpoint), _shell(shell) {}

    void detachWindow(Strata::IShell::WindowId id) {
        (void)_endpoint->send(_shell, Strata::IShell::WindowDestroy{id});
        _windows.del((usize)id);
    }

    Async::Task<Rc<Window>> createWindowAsync(WindowProps const& props, Async::CancellationToken ct) override {
        // NOTE: The Window API is synchronous, but Strata is async.
        // We use Sys::run to block until the window is created by the shell.
        auto [id, actual] = co_trya$(_endpoint->callAsync(
            _shell,
            Strata::IShell::WindowCreate{
                props.size,
                App::formFactor
            },
            ct
        ));

        formFactor = actual.formFactor;

        auto surface = co_try$(Strata::Protos::Surface::create(actual.size));

        (void)_endpoint->send(
            _shell,
            Strata::IShell::WindowAttach{
                id,
                surface,
            }
        );

        auto window = makeRc<SkiftWindow>(*this, id, surface);
        _windows.put(id, &*window);

        co_return Ok(window);
    }

    void pollMessage(Rc<Handler> handler) {
        while (auto maybeMessage = _endpoint->tryRecv()) {
            if (maybeMessage->is<Strata::IShell::WindowEvent>()) {
                auto m = maybeMessage->unpack<Strata::IShell::WindowEvent>().unwrap();
                auto windowId = WindowId{m.window};

                m.event.visit([&](auto const& event) {
                    using T = Meta::RemoveConstVolatileRef<decltype(event)>;

                    if constexpr (Meta::Contains<T, MouseEvent, KeyboardEvent, TypeEvent>) {
                        handler->handle<T>(windowId, event);
                    } else if constexpr (Meta::Same<T, RequestCloseEvent>) {
                        _exited = true;
                    }
                });
            }
        }
    }

    bool exited() {
        for (auto [id, window] : _windows.iterUnordered())
            if (not window->_closed)
                return _exited;
        return true;
    }

    Async::Task<> runAsync(Rc<Handler> handler, Async::CancellationToken ct) override {
        Duration const frameDuration = Duration::fromMSecs(16);
        Instant nextFrameTime = Sys::instant();

        while (not exited()) {
            nextFrameTime = nextFrameTime + frameDuration;

            pollMessage(handler);

            handler->update();

            for (auto [id, window] : _windows.iterUnordered()) {
                if (not window->_dirty)
                    continue;

                co_trya$(_endpoint->callAsync(
                    _shell,
                    Strata::IShell::WindowFlip{
                        id,
                        window->bound(),
                    },
                    ct
                ));

                window->_dirty = false;
            }

            co_trya$(Sys::globalSched().sleepAsync(nextFrameTime, ct));

            Instant now = Sys::instant();
            if (now > nextFrameTime + frameDuration) {
                nextFrameTime = now;
            }
        }
        co_return Ok();
    }
};

SkiftWindow::~SkiftWindow() {
    _application.detachWindow(_id);
}

void SkiftWindow::releaseSurface(Slice<Math::Recti>) {
}

void SkiftWindow::drag(DragEvent e) {
    if (e.type == DragEvent::START)
        (void)_application._endpoint->send(_application._shell, Strata::IShell::WindowMove{_id});
}

Async::Task<Rc<Application>> createAppAsync(Sys::Context& ctx, ApplicationProps const&, Async::CancellationToken ct) {
    auto endpoint = makeRc<Sys::Endpoint>(
        std::move(Sys::useChannel(ctx).con)
    );

    auto maybeShellPort = co_await endpoint->callAsync(
        Sys::Port::BUS,
        Strata::IBus::Locate{
            "strata-shell"s,
        },
        ct
    );

    if (not maybeShellPort)
        co_return maybeShellPort.none();

    co_return Ok(makeRc<SkiftApplication>(endpoint, maybeShellPort.unwrap()));
}

} // namespace Karm::App::_Embed

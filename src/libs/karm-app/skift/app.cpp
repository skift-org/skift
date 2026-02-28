module;

#include <karm/macros>

module Karm.App;

import Karm.Core;
import Karm.Gfx;
import Karm.Sys;
import Karm.Ref;
import Strata.Protos;
import Karm.Logger;

namespace Karm::App::_Embed {

struct SkiftApplication;

struct SkiftWindow : Window {
    SkiftApplication& _application;
    Strata::IShell::WindowId _id;
    Rc<Strata::Protos::Surface> _surface;
    Opt<Math::Recti> _dirty = NONE;
    bool _closed = false;
    Opt<Math::Vec2i> _shouldResize;

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
        return _surface->mutPixels();
    }

    void releaseSurface(Slice<Math::Recti> dirty) override {
        if (dirty.len() == 0)
            return;
        Math::Recti merged = dirty[0];
        for (auto r : next(dirty, 1))
            merged = merged.mergeWith(r);
        _dirty = merged;
    }

    void drag() override;

    void snap(Snap snap) override;

    void minimize() override {}

    void close() override {
        _closed = true;
    }
};

struct SkiftApplication : Application {
    Sys::IpcClient _shell;
    Map<usize, SkiftWindow*> _windows;
    bool _exited = false;

    explicit SkiftApplication(Sys::IpcClient shell)
        : _shell(std::move(shell)) {}

    void detachWindow(Strata::IShell::WindowId id) {
        (void)_shell.notify(Strata::IShell::WindowDestroy{id});
        (void)_windows.remove((usize)id);
    }

    Async::Task<Rc<Window>> createWindowAsync(WindowProps const& props, Async::CancellationToken ct) override {
        auto [id, actual] = co_trya$(_shell.callAsync(
            Strata::IShell::WindowCreate{
                props.size,
                App::formFactor
            },
            ct
        ));

        formFactor = actual.formFactor;

        auto surface = co_try$(Strata::Protos::Surface::create(actual.size));

        co_trya$(_shell.callAsync(
            Strata::IShell::WindowAttach{
                id,
                surface,
            },
            ct
        ));

        auto window = makeRc<SkiftWindow>(*this, id, surface);
        _windows.put(id, &*window);

        co_return Ok(window);
    }

    Res<> _handleWindowEvent(Sys::IpcMessage& message, Rc<Handler> handler) {
        auto [windowId, payload] = try$(message.unpack<Strata::IShell::WindowEvent>());
        payload.visit([&]<typename E>(E const& event) {
            using T = Meta::RemoveConstVolatileRef<E>;

            if constexpr (Meta::Contains<T, MouseEvent, KeyboardEvent>) {
                handler->handle<T>(App::WindowId{windowId}, event);
            } else if constexpr (Meta::Same<T, RequestCloseEvent>) {
                _exited = true;
            }
        });
        return Ok();
    }

    Res<> _handleWindowUpdate(Sys::IpcMessage& message, Rc<Handler> handler) {
        auto [windowId, props] = try$(message.unpack<Strata::IShell::WindowUpdate>());
        auto* window = try$(_windows.lookup(windowId).okOr(Error::invalidInput("no such window")));
        // FIXME: We should probably have a separated hello message that pass stuff like color scheme and form factor
        if (App::formFactor != props.formFactor) {
            App::formFactor = props.formFactor;
            handler->handle<App::FormfactorEvent>(App::GLOBAL, props.formFactor);
        }
        window->_shouldResize = props.size;
        return Ok();
    }

    Res<> _pollMessages(Rc<Handler> handler) {
        while (auto maybeMessage = _shell.poll()) {
            auto& message = maybeMessage.unwrap();
            if (message.is<Strata::IShell::WindowEvent>())
                try$(_handleWindowEvent(message, handler));
            else if (message.is<Strata::IShell::WindowUpdate>())
                try$(_handleWindowUpdate(message, handler));
        }
        return Ok();
    }

    bool exited() {
        for (auto [id, window] : _windows.iterItems())
            if (not window->_closed)
                return _exited;
        return true;
    }

    Async::Task<> _handleWindowResizeAsync(Rc<Handler> handler, Async::CancellationToken ct) {
        for (auto [id, window] : _windows.iterItems()) {
            if (not window->_shouldResize)
                continue;

            auto size = window->_shouldResize.unwrap();

            auto newSurface = co_try$(Strata::Protos::Surface::create(size));

            co_trya$(_shell.callAsync(
                Strata::IShell::WindowAttach{
                    id,
                    newSurface,
                },
                ct
            ));

            window->_surface = newSurface;
            window->_shouldResize = NONE;
            handler->handle<ResizeEvent>(window->id(), size);
        }

        co_return Ok();
    }

    Async::Task<> runAsync(Rc<Handler> handler, Async::CancellationToken ct) override {
        Duration const frameDuration = Duration::fromMSecs(16);
        Instant nextFrameTime = Sys::instant();

        while (not exited()) {
            nextFrameTime = nextFrameTime + frameDuration;

            if (auto res = _pollMessages(handler); not res) {
                logError("failed to pull message from server: {}", res);
            }

            co_trya$(_handleWindowResizeAsync(handler, ct));
            handler->update();

            for (auto [id, window] : _windows.iterItems()) {
                if (not window->_dirty)
                    continue;

                co_trya$(_shell.callAsync(
                    Strata::IShell::WindowFlip{
                        id,
                        window->_dirty.unwrap(),
                    },
                    ct
                ));

                window->_dirty = NONE;
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

void SkiftWindow::drag() {
    (void)_application._shell.notify(Strata::IShell::WindowMove{_id});
}

void SkiftWindow::snap(Snap s) {
    (void)_application._shell.notify(Strata::IShell::WindowSnap{_id, s});
}

Async::Task<Rc<Application>> createAppAsync(Sys::Context&, ApplicationProps const&, Async::CancellationToken ct) {
    auto shell = co_trya$(Sys::IpcClient::connectAsync("ipc://strata-shell"_url, ct));
    co_return Ok(makeRc<SkiftApplication>(std::move(shell)));
}

} // namespace Karm::App::_Embed

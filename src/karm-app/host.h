#pragma once

#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>
#include <karm-events/events.h>
#include <karm-gfx/surface.h>
#include <karm-meta/utils.h>

#include "client.h"
#include "karm-math/rect.h"

namespace Karm::App {

struct Host : public Meta::Static {
    bool _alive{};
    Error _result{OK};
    Gfx::Context _gfx{};
    OptBox<Client> _client{};

    bool _shouldRepaint{};
    bool _shouldLayout{};
    bool _shouldAnimate{};

    Host() = default;

    virtual ~Host() {
        if (_client) {
            _client.unwrap()->unmount();
            _client = NONE;
        }
    };

    virtual Gfx::Surface surface() = 0;

    virtual void flip(Slice<Math::Recti> regions) = 0;

    virtual void pump() = 0;

    virtual void wait(size_t ms) = 0;

    /* --- Lifecycle -------------------------------------------------------- */

    void mount(Box<Client> client) {
        _client = std::move(client);
        _client.unwrap()->mount(*this);
    }

    void unmount() {
        _client.with([&](auto &c) {
            c->unmount();
        });
    }

    void handle(Events::Event &event) {
        _client.with([&](auto &c) {
            c->onEvent(event);
        });
    }

    void layout() {
        _client.with([&](auto &c) {
            c->onLayout(surface().bound());
        });
    }

    void paint() {
        Gfx::Surface s = surface();

        _gfx.begin(s);

        _client.with([&](auto &c) {
            c->onPaint(_gfx);
        });

        _gfx.end();

        Array<Math::Recti, 1> dirty = {s.bound()};
        flip(dirty);
    }

    Error run() {
        _alive = true;

        surface().clear(Gfx::ZINC900);
        paint();

        while (_alive) {
            wait(_shouldAnimate ? 16 : -1);

            if (_shouldAnimate) {
                Events::AnimationEvent e;
                handle(e);
                _shouldAnimate = false;
            }

            pump();

            if (_shouldLayout) {
                layout();
                _shouldLayout = false;
                _shouldRepaint = true;
            }

            if (_shouldRepaint) {
                paint();
                _shouldRepaint = false;
            }
        }

        return _result;
    }

    Math::Recti bound() {
        return surface().bound();
    }

    void exit(Error result = OK) {
        _alive = false;
        _result = result;
    }

    void shouldRepaint(Math::Recti) {
        _shouldRepaint = true;
    }

    void shouldLayout() {
        _shouldLayout = true;
    }

    void shouldAnimate() {
        _shouldAnimate = true;
    }
};

Result<Strong<Karm::App::Host>> makeHost();

template <typename T, typename... Args>
static inline Error run(Args &&...args) {
    auto host = try$(makeHost());
    auto client = makeBox<T>(std::forward<Args>(args)...);
    host->mount(std::move(client));
    return host->run();
}

} // namespace Karm::App

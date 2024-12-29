#include <handover/hook.h>
#include <hideo-shell/app.h>
#include <hjert-api/api.h>
#include <karm-app/host.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-gfx/cpu/surface.h>
#include <karm-image/loader.h>
#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>
#include <karm-sys/time.h>
#include <karm-ui/perf.h>
#include <mdi/calculator.h>
#include <mdi/cog.h>
#include <mdi/counter.h>
#include <mdi/duck.h>
#include <mdi/emoticon.h>
#include <mdi/file.h>
#include <mdi/format-font.h>
#include <mdi/image.h>
#include <mdi/information-outline.h>
#include <mdi/palette-swatch.h>
#include <mdi/table.h>
#include <mdi/widgets.h>

#include "../grund-bus/api.h"

namespace Grund::Shell {

struct Framebuffer : public Gfx::CpuSurface {
    static Res<Strong<Framebuffer>> open(Sys::Context &ctx) {
        auto &handover = useHandover(ctx);
        auto *fb = handover.findTag(Handover::Tag::FB);
        auto fbVmo = try$(Hj::Vmo::create(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
        try$(fbVmo.label("framebuffer"));
        return Ok(makeStrong<Framebuffer>(*fb, try$(Hj::map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE))));
    }

    Handover::Record _record;
    Hj::Mapped _map;

    Framebuffer(Handover::Record record, Hj::Mapped map)
        : _record(record), _map(std::move(map)) {
        logInfo(
            "fb: {x}-{x} {}x{}, {} stride",
            _map.range().start,
            _map.range().end(),
            _record.fb.width,
            _record.fb.height,
            _record.fb.pitch
        );
    }

    Gfx::MutPixels mutPixels() {
        return {
            _map.mutBytes().buf(),
            {
                _record.fb.width,
                _record.fb.height,
            },
            _record.fb.pitch,
            Gfx::BGRA8888,
        };
    }
};

struct InputTranslator : public Ui::ProxyNode<InputTranslator> {
    App::MouseEvent _mousePrev = {};
    Math::Vec2i _mousePos = {};
    bool _mouseDirty = false;

    InputTranslator(Ui::Child child)
        : Ui::ProxyNode<InputTranslator>(std::move(child)) {
    }

    Math::Ellipsef _cursor() const {
        return {
            _mousePos.cast<f64>(),
            {16, 16},
        };
    }

    Math::Recti _cursorDamage() const {
        return _cursor().bound().grow(1).cast<isize>();
    }

    void event(App::Event &e) override {
        if (auto m = e.is<App::MouseEvent>()) {
            if (not _mouseDirty) {
                _mouseDirty = true;
                Ui::shouldRepaint(*this, _cursorDamage());
                Ui::shouldAnimate(*this);
            }

            _mousePos = _mousePos + m->delta;
            _mousePos = _mousePos.min(bound().size() - Math::Vec2i{1, 1});
            _mousePos = _mousePos.max(Math::Vec2i{0, 0});
            e.accept();

            if (m->delta != Math::Vec2i{}) {
                App::MouseEvent mouseMove = *m;
                mouseMove.type = App::MouseEvent::MOVE;
                mouseMove.pos = _mousePos;
                Ui::event<App::MouseEvent>(child(), mouseMove);
            }

            if (_mousePrev.released(App::MouseButton::LEFT) and
                m->pressed(App::MouseButton::LEFT)) {
                App::MouseEvent mousePress = *m;
                mousePress.type = App::MouseEvent::PRESS;
                mousePress.pos = _mousePos;
                mousePress.button = App::MouseButton::LEFT;
                Ui::event<App::MouseEvent>(child(), mousePress);
            }

            if (_mousePrev.pressed(App::MouseButton::LEFT) and
                m->released(App::MouseButton::LEFT)) {
                App::MouseEvent mouseRelease = *m;
                mouseRelease.type = App::MouseEvent::RELEASE;
                mouseRelease.pos = _mousePos;
                mouseRelease.button = App::MouseButton::LEFT;
                Ui::event<App::MouseEvent>(child(), mouseRelease);
            }

            _mousePrev = *m;
        } else if (auto k = e.is<Node::AnimateEvent>()) {
            if (_mouseDirty) {
                _mouseDirty = false;
                Ui::shouldRepaint(*this, _cursorDamage());
            }
        }

        Ui::ProxyNode<InputTranslator>::event(e);
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        child().paint(g, r);

        if (_cursorDamage().colide(r)) {
            g.push();
            g.beginPath();
            g.fillStyle(Gfx::WHITE.withOpacity(0.25));
            g.ellipse(_cursor());
            g.fill(Gfx::FillRule::EVENODD);
            g.pop();
        }
    }
};

Ui::Child inputTranslator(Ui::Child child) {
    return makeStrong<InputTranslator>(std::move(child));
}

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
            // g.plot(r, Gfx::RED);
            g.pop();

            Gfx::blitUnsafe(_frontbuffer->mutPixels(), _backbuffer->pixels());
        }
        g.end();

        _dirty.clear();
    }

    Async::Task<> run() {
        _shouldLayout = true;

        while (true) {
            auto e = App::makeEvent<Node::AnimateEvent>(Ui::FRAME_TIME);
            event(*e);

            if (_shouldLayout) {
                layout(bound());
                _shouldLayout = false;
                _dirty.clear();
                _dirty.pushBack(_backbuffer->bound());
            }

            if (_dirty.len() > 0) {
                _repaint();
            }

            co_trya$(Sys::globalSched().sleepAsync(Sys::now() + 16_ms));
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
            makeStrong<Hideo::Shell::Manifest>(Mdi::INFORMATION_OUTLINE, "About"s, Gfx::BLUE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::CALCULATOR, "Calculator"s, Gfx::ORANGE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::PALETTE_SWATCH, "Color Picker"s, Gfx::RED_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::COUNTER, "Counter"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::DUCK, "Demos"s, Gfx::YELLOW_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::FILE, "Files"s, Gfx::ORANGE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::FORMAT_FONT, "Fonts"s, Gfx::BLUE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::EMOTICON, "Hello World"s, Gfx::RED_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::IMAGE, "Icons"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::IMAGE, "Image Viewer"s, Gfx::YELLOW_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::COG, "Settings"s, Gfx::ZINC_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::TABLE, "Spreadsheet"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::WIDGETS, "Widget Gallery"s, Gfx::BLUE_RAMP),
        },
        .instances = {}
    };

    auto rpc = Sys::Rpc::create(ctx);
    auto root = makeStrong<Root>(
        Hideo::Shell::app(std::move(state)) | inputTranslator,
        co_try$(Framebuffer::open(ctx))
    );

    Async::detach(root->run());

    co_try$(rpc.send<Grund::Bus::Listen>(Sys::Port::BUS, Meta::idOf<App::MouseEvent>()));
    co_try$(rpc.send<Grund::Bus::Listen>(Sys::Port::BUS, Meta::idOf<App::KeyboardEvent>()));

    while (true) {
        auto msg = co_trya$(rpc.recvAsync());

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

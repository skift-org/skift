#include <karm-math/path.h>

#include "input.h"

import Karm.Ui;
import Karm.Gfx;
import Karm.App;

namespace Strata::Shell {

struct Cursor {
    virtual ~Cursor() = default;

    virtual Math::Recti bound(Math::Vec2i pos) const = 0;

    virtual void paint(Math::Vec2i pos, Gfx::Canvas& g) const = 0;
};

struct TouchCursor : Cursor {
    Gfx::Color _fill = Gfx::WHITE;
    f64 _opacity = 0.25;

    Math::Ellipsef _blob(Math::Vec2i pos) const {
        return {
            pos.cast<f64>(),
            {16, 16},
        };
    }

    Math::Recti bound(Math::Vec2i pos) const override {
        return _blob(pos).bound().grow(1).cast<isize>();
    }

    void paint(Math::Vec2i pos, Gfx::Canvas& g) const override {
        g.push();
        g.beginPath();
        g.fillStyle(_fill.withOpacity(_opacity));
        g.ellipse(_blob(pos));
        g.fill(Gfx::FillRule::EVENODD);
        g.pop();
    }
};

struct ClassicCursor : Cursor {
    Gfx::Color _fill = Gfx::BLACK;
    Gfx::Color _stroke = Gfx::WHITE;
    Math::Path _path = Math::Path::fromSvg(
#include "hideo-shell/defs/classic-cursor.path"

    );

    Math::Recti bound(Math::Vec2i pos) const override {
        return Math::Recti{
            pos,
            {28, 28},
        }
            .grow(1);
    }

    void paint(Math::Vec2i pos, Gfx::Canvas& g) const override {
        g.push();
        g.translate(pos.cast<f64>());
        g.beginPath();
        g.path(_path);
        g.fill(_fill);
        g.stroke({
            .fill = _stroke,
            .width = 1,
            .align = Gfx::OUTSIDE_ALIGN,
            .join = Gfx::MITER_JOIN,
        });
        g.pop();
    }
};

struct RoundedCursor : Cursor {
    Gfx::Color _fill = Gfx::BLACK;
    Gfx::Color _stroke = Gfx::WHITE;
    Math::Path _path = Math::Path::fromSvg(
#include "hideo-shell/defs/rounded-cursor.path"

    );

    Math::Recti bound(Math::Vec2i pos) const override {
        return Math::Recti{
            pos,
            {28, 28},
        }
            .grow(1);
    }

    void paint(Math::Vec2i pos, Gfx::Canvas& g) const override {
        g.push();
        g.translate(pos.cast<f64>());
        g.beginPath();
        g.path(_path);
        g.fill(_fill);
        g.stroke({
            .fill = _stroke,
            .width = 1.6,
            .align = Gfx::CENTER_ALIGN,
            .join = Gfx::MITER_JOIN,
        });
        g.pop();
    }
};

struct InputTranslator : Ui::ProxyNode<InputTranslator> {
    App::MouseEvent _mousePrev = {};
    Math::Vec2i _mousePos = {};
    bool _mouseDirty = false;
    Box<Cursor> _cursor;

    InputTranslator(Ui::Child child, Math::Vec2i mousePos, Box<Cursor> cursor)
        : Ui::ProxyNode<InputTranslator>(std::move(child)), _mousePos(mousePos), _cursor(std::move(cursor)) {}

    void event(App::Event& e) override {
        if (auto m = e.is<App::MouseEvent>()) {
            if (not _mouseDirty) {
                _mouseDirty = true;
                Ui::shouldRepaint(*this, _cursor->bound(_mousePos));
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
                Ui::shouldRepaint(*this, _cursor->bound(_mousePos));
            }
        }

        Ui::ProxyNode<InputTranslator>::event(e);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        child().paint(g, r);

        if (_cursor->bound(_mousePos).colide(r)) {
            _cursor->paint(_mousePos, g);
        }
    }
};

Ui::Child inputTranslator(Ui::Child child, Math::Vec2i mousePos) {
    return makeRc<InputTranslator>(std::move(child), mousePos, makeBox<RoundedCursor>());
}

} // namespace Strata::Shell

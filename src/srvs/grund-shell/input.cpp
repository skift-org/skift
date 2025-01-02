#include <karm-app/inputs.h>
#include <karm-ui/funcs.h>

#include "input.h"

namespace Grund::Shell {

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

} // namespace Grund::Shell

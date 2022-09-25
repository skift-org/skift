#include "button.h"

#include "funcs.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

ButtonStyle ButtonStyle::regular() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };
}

ButtonStyle ButtonStyle::secondary() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundColor = Gfx::ZINC800,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC700,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
            .backgroundColor = Gfx::ZINC800,
        },
    };
}

ButtonStyle ButtonStyle::primary() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundColor = Gfx::BLUE700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::BLUE600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::BLUE600,
            .backgroundColor = Gfx::BLUE700,
        },
    };
}

ButtonStyle ButtonStyle::outline() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };
}

ButtonStyle ButtonStyle::subtle() {
    return {
        .idleStyle = {
            .foregroundColor = Gfx::ZINC300,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };
}

ButtonStyle ButtonStyle::destructive() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .foregroundColor = Gfx::RED500,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundColor = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::RED600,
            .backgroundColor = Gfx::RED700,
        },
    };
}

ButtonStyle ButtonStyle::withRadius(float radius) const {
    return {
        idleStyle.withRadius(radius),
        hoverStyle.withRadius(radius),
        pressStyle.withRadius(radius),
    };
}

enum ButtonState {
    IDLE,
    OVER,
    PRESS,
};

struct Button : public _Box<Button> {

    OnPress _onPress;
    ButtonStyle _buttonStyle = ButtonStyle::regular();
    ButtonState _state = IDLE;

    Button(OnPress onPress, ButtonStyle style, Child child)
        : _Box<Button>(child),
          _onPress(std::move(onPress)),
          _buttonStyle(style) {}

    void reconcile(Button &o) override {
        _buttonStyle = o._buttonStyle;
        _onPress = std::move(o._onPress);
        _Box<Button>::reconcile(o);
    }

    BoxStyle &boxStyle() override {
        if (!_onPress) {
            return _buttonStyle.disabledStyle;
        }

        if (_state == IDLE) {
            return _buttonStyle.idleStyle;
        } else if (_state == OVER) {
            return _buttonStyle.hoverStyle;
        } else {
            return _buttonStyle.pressStyle;
        }
    }

    void event(Events::Event &e) override {
        ButtonState state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (!bound().contains(m.pos)) {
                state = IDLE;
                return false;
            }

            if (state != PRESS) {
                state = OVER;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                state = PRESS;
                return true;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                state = OVER;
                if (_onPress) {
                    _onPress->operator()(*this);
                }
                return true;
            }

            return false;
        });

        if (state != _state) {
            _state = state;
            shouldRepaint(*this);
        }
    };
};

Child button(OnPress onPress, ButtonStyle style, Child child) {
    return makeStrong<Button>(std::move(onPress), style, child);
}

Child button(OnPress onPress, ButtonStyle style, Str t) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {UNCONSTRAINED, 36},
            center(
                spacing(
                    {16, 6},
                    text(t)))));
}

Child button(OnPress onPress, ButtonStyle style, int size, Str t) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {UNCONSTRAINED, 36},
            center(
                spacing(
                    {16, 6},
                    text(size, t)))));
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {36, 36},
            center(
                spacing(
                    {6, 6},
                    icon(i)))));
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t) {
    return button(
        std::move(onPress),
        style,
        minSize(
            {36, 36},
            spacing(
                {12, 6, 16, 6},
                hflow(
                    8,
                    Layout::Align::CENTER,
                    icon(i),
                    text(t)))));
}

Child button(OnPress onPress, Child child) {
    return button(std::move(onPress), ButtonStyle::regular(), child);
}

Child button(OnPress onPress, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), t);
}

Child button(OnPress onPress, int size, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), size, t);
}

Child button(OnPress onPress, Media::Icons i) {
    return button(std::move(onPress), ButtonStyle::regular(), i);
}

Child button(OnPress onPress, Media::Icons i, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), i, t);
}

} // namespace Karm::Ui

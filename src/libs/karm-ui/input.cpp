#include "input.h"

#include "funcs.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

/* --- Button ---------------------------------------------------------------- */

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
            .borderColor = Gfx::ZINC700,
            .backgroundColor = Gfx::ZINC700,
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

ButtonStyle ButtonStyle::withRadius(Gfx::BorderRadius radius) const {
    return {
        idleStyle.withRadius(radius),
        hoverStyle.withRadius(radius),
        pressStyle.withRadius(radius),
    };
}

ButtonStyle ButtonStyle::withForegroundColor(Gfx::Color color) const {
    return {
        idleStyle.withForegroundColor(color),
        hoverStyle.withForegroundColor(color),
        pressStyle.withForegroundColor(color),
    };
}

struct Button : public _Box<Button> {
    OnPress _onPress;
    ButtonStyle _buttonStyle = ButtonStyle::regular();
    MouseListener _mouseListener;

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

        if (_mouseListener.isIdle()) {
            return _buttonStyle.idleStyle;
        } else if (_mouseListener.isHover()) {
            return _buttonStyle.hoverStyle;
        } else {
            return _buttonStyle.pressStyle;
        }
    }

    void event(Events::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _onPress(*this);
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

Child button(OnPress onPress, Media::Icons i) {
    return button(std::move(onPress), ButtonStyle::regular(), i);
}

Child button(OnPress onPress, Media::Icons i, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), i, t);
}

/* --- Toggle --------------------------------------------------------------- */

struct Toggle : public View<Toggle> {
    bool _value = false;
    OnChange<bool> _onChange;
    MouseListener _mouseListener;

    Toggle(bool value, OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Toggle &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();

        auto thumb = bound().hsplit(26).get(_value).shrink(_value ? 4 : 6);

        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Gfx::BLUE600 : Gfx::BLUE700);
            g.fill(bound(), 999);

            g.fillStyle(Gfx::WHITE);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::BLUE600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Gfx::ZINC600 : Gfx::ZINC700);
            g.fill(bound(), 999);

            g.fillStyle(_mouseListener.isHover() ? Gfx::ZINC400 : Gfx::ZINC500);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        }

        g.restore();
    }

    void event(Events::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = !_value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {52, 26};
    }
};

Child toggle(bool value, OnChange<bool> onChange) {
    return makeStrong<Toggle>(value, std::move(onChange));
}

/* --- Checkbox ------------------------------------------------------------- */

struct Checkbox : public View<Checkbox> {
    bool _value = false;
    OnChange<bool> _onChange;
    MouseListener _mouseListener;

    Checkbox(bool value, OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Checkbox &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();

        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Gfx::BLUE600 : Gfx::BLUE700);
            g.fill(bound(), 4);

            g.fillStyle(Gfx::WHITE);
            g.fill(bound().topStart(), Media::Icon{Media::Icons::CHECK_BOLD, 26});

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::BLUE600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 4);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Gfx::ZINC600 : Gfx::ZINC700);
            g.fill(bound(), 4);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 4);
            }
        }

        g.restore();
    }

    void event(Events::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = !_value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {26, 26};
    }
};

Child checkbox(bool value, OnChange<bool> onChange) {
    return makeStrong<Checkbox>(value, std::move(onChange));
}

/* --- Radio ----------------------------------------------------------------- */

struct Radio : public View<Radio> {
    bool _value = false;
    OnChange<bool> _onChange;
    MouseListener _mouseListener;

    Radio(bool value, OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Radio &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Gfx::BLUE600 : Gfx::BLUE700);
            g.fill(bound(), 999);

            g.fillStyle(Gfx::WHITE);
            g.fill(bound().shrink(6), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::BLUE600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Gfx::ZINC600 : Gfx::ZINC700);
            g.fill(bound(), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        }
        g.restore();
    }

    void event(Events::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = !_value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {26, 26};
    }
};

Child radio(bool value, OnChange<bool> onChange) {
    return makeStrong<Radio>(value, std::move(onChange));
}

} // namespace Karm::Ui

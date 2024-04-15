#include "input.h"

#include "drag.h"
#include "funcs.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

// MARK: Button -----------------------------------------------------------------

ButtonStyle ButtonStyle::none() {
    return {};
}

ButtonStyle ButtonStyle::regular(Gfx::ColorRamp ramp) {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundPaint = ramp[8],
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = ramp[7],
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = ramp[7],
            .backgroundPaint = ramp[8],
        },
    };
}

ButtonStyle ButtonStyle::secondary() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundPaint = GRAY900,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = GRAY800,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = GRAY800,
            .backgroundPaint = GRAY900,
        },
    };
}

ButtonStyle ButtonStyle::primary() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .backgroundPaint = ACCENT700,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = ACCENT600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = ACCENT600,
            .backgroundPaint = ACCENT700,
        },
    };
}

ButtonStyle ButtonStyle::outline() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = GRAY800,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = GRAY700,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = GRAY700,
            .backgroundPaint = GRAY800,
        },
    };
}

ButtonStyle ButtonStyle::subtle() {
    return {
        .idleStyle = {
            .foregroundPaint = GRAY300,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = GRAY700,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = GRAY700,
            .backgroundPaint = GRAY800,
        },
    };
}

ButtonStyle ButtonStyle::text() {
    return {
        .idleStyle = {
            .foregroundPaint = GRAY300,
        },
        .pressStyle = {
            .foregroundPaint = GRAY300,
        },
    };
}

ButtonStyle ButtonStyle::destructive() {
    return {
        .idleStyle = {
            .borderRadius = RADIUS,
            .foregroundPaint = Gfx::RED500,
        },
        .hoverStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .backgroundPaint = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadius = RADIUS,
            .borderWidth = 1,
            .borderPaint = Gfx::RED600,
            .backgroundPaint = Gfx::RED700,
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

ButtonStyle ButtonStyle::withForegroundPaint(Gfx::Paint paint) const {
    return {
        idleStyle.withForegroundPaint(paint),
        hoverStyle.withForegroundPaint(paint),
        pressStyle.withForegroundPaint(paint),
    };
}

ButtonStyle ButtonStyle::withPadding(Layout::Spacingi spacing) const {
    return {
        idleStyle.withPadding(spacing),
        hoverStyle.withPadding(spacing),
        pressStyle.withPadding(spacing),
    };
}

ButtonStyle ButtonStyle::withMargin(Layout::Spacingi spacing) const {
    return {
        idleStyle.withMargin(spacing),
        hoverStyle.withMargin(spacing),
        pressStyle.withMargin(spacing),
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

        if (!_onPress) {
            // Reset the mouse listener if the button is disabled.
            _mouseListener = {};
        }

        _Box<Button>::reconcile(o);
    }

    BoxStyle &boxStyle() override {
        if (not _onPress) {
            return _buttonStyle.disabledStyle;
        } else if (_mouseListener.isIdle()) {
            return _buttonStyle.idleStyle;
        } else if (_mouseListener.isHover()) {
            return _buttonStyle.hoverStyle;
        } else {
            return _buttonStyle.pressStyle;
        }
    }

    void event(Sys::Event &e) override {
        if (_onPress and _mouseListener.listen(*this, e)) {
            _onPress(*this);
        }
    };
};

Child button(OnPress onPress, ButtonStyle style, Child child) {
    return makeStrong<Button>(std::move(onPress), style, child);
}

Child button(OnPress onPress, ButtonStyle style, Str t) {
    return text(t) |
           spacing({16, 6}) |
           center() |
           minSize({UNCONSTRAINED, 36}) |
           button(std::move(onPress), style);
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i) {
    return icon(i) |
           spacing({6, 6}) |
           center() |
           minSize({36, 36}) |
           button(std::move(onPress), style);
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t) {
    return hflow(8, Layout::Align::CENTER, icon(i), text(t)) |
           spacing({12, 6, 16, 6}) |
           minSize({UNCONSTRAINED, 36}) |
           button(std::move(onPress), style);
}

Child button(OnPress onPress, Child child) {
    return button(std::move(onPress), ButtonStyle::regular(), child);
}

Child button(OnPress onPress, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), t);
}

Child button(OnPress onPress, Mdi::Icon i) {
    return button(std::move(onPress), ButtonStyle::regular(), i);
}

Child button(OnPress onPress, Mdi::Icon i, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), i, t);
}

// MARK: Input -----------------------------------------------------------------

struct Input : public View<Input> {
    Gfx::TextStyle _style;

    Strong<Textbox::Model> _model;
    OnChange<Textbox::Action> _onChange;

    Opt<Gfx::Text> _text;

    Input(Gfx::TextStyle style, Strong<Textbox::Model> model, OnChange<Textbox::Action> onChange)
        : _style(style), _model(model), _onChange(std::move(onChange)) {}

    void reconcile(Input &o) override {
        _style = o._style;
        _model = o._model;
        _onChange = std::move(o._onChange);

        // NOTE: The model might have changed,
        //       so we need to invalidate the presentation.
        _text = NONE;
    }

    Gfx::Text &_ensureText() {
        if (not _text) {
            _text = Gfx::Text(_style);
            auto runes = _model->runes();
            auto cur = _model->_cur;

            for (usize i = 0; i < runes.len(); i++) {
                if (not cur.open() and i == cur.head) {
                    _text->append('|');
                } else if (cur.open() and i == min(cur.tail, cur.head)) {
                    _text->append('[');
                } else if (cur.open() and i == max(cur.tail, cur.head)) {
                    _text->append(']');
                }
                _text->append(runes[i]);
            }

            if (not cur.open() and cur.head == runes.len()) {
                _text->append('|');
            } else if (cur.open() and max(cur.tail, cur.head) == runes.len()) {
                _text->append(']');
            }
        }
        return *_text;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(bound());
        g.origin(bound().xy);

        auto &text = _ensureText();
        text.paint(g);

        g.restore();
        if (debugShowLayoutBounds)
            g.plot(bound(), Gfx::CYAN);
    }

    void event(Sys::Event &e) override {
        auto a = Textbox::Action::fromEvent(e);
        if (a) {
            e.accept();
            _onChange(*this, *a);
        }
    }

    void layout(Math::Recti bound) override {
        _ensureText().layout(bound.width);
        View<Input>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint) override {
        auto size = _ensureText().layout(s.width);
        return size.ceil().cast<isize>();
    }
};

Child input(Gfx::TextStyle style, Strong<Textbox::Model> text, OnChange<Textbox::Action> onChange) {
    return makeStrong<Input>(style, text, std::move(onChange));
}

Child input(Strong<Textbox::Model> text, OnChange<Textbox::Action> onChange) {
    return makeStrong<Input>(TextStyles::bodyMedium(), text, std::move(onChange));
}

// MARK: Toggle ----------------------------------------------------------------

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

        auto thumb =
            bound()
                .hsplit(26)
                .get(_value)
                .shrink(_value ? 4 : 6);

        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? ACCENT600 : ACCENT700);
            g.fill(bound(), 999);

            g.fillStyle(GRAY50);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(ACCENT600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? GRAY600 : GRAY700);
            g.fill(bound(), 999);

            g.fillStyle(_mouseListener.isHover() ? GRAY400 : GRAY500);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(GRAY600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        }

        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
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

// MARK: Checkbox --------------------------------------------------------------

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
            g.fillStyle(_mouseListener.isHover() ? ACCENT600 : ACCENT700);
            g.fill(bound(), 4);

            g.fillStyle(Gfx::GRAY50);
            g.fill(bound().topStart(), Media::Icon{Mdi::CHECK_BOLD, 26});

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(ACCENT600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 4);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? GRAY600 : GRAY700);
            g.fill(bound(), 4);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(GRAY600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 4);
            }
        }

        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
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

// MARK: Radio ------------------------------------------------------------------

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
            g.fillStyle(_mouseListener.isHover() ? ACCENT600 : ACCENT700);
            g.fill(bound(), 999);

            g.fillStyle(Gfx::GRAY50);
            g.fill(bound().shrink(6), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(ACCENT600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? GRAY600 : GRAY700);
            g.fill(bound(), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(GRAY600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        }
        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
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

// MARK: Slider -----------------------------------------------------------------

struct Slider : public ProxyNode<Slider> {
    f64 _value = 0.0f;
    OnChange<f64> _onChange;
    Math::Recti _bound;

    Slider(f64 value, OnChange<f64> onChange, Child child)
        : ProxyNode<Slider>(std::move(child)),
          _value(value),
          _onChange(std::move(onChange)) {
    }

    void reconcile(Slider &o) override {
        _value = o._value;
        _onChange = o._onChange;

        ProxyNode<Slider>::reconcile(o);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        child().layout(_bound.hsplit(((r.width - r.height) * _value) + r.height).car);
    }

    Math::Recti bound() override {
        return _bound;
    }

    void bubble(Sys::Event &e) override {
        if (auto *dv = e.is<DragEvent>()) {
            if (dv->type == DragEvent::DRAG) {
                auto max = bound().width - bound().height;
                auto value = max * _value;
                value = clamp(value + dv->delta.x, 0.0f, max);
                _value = value / max;
                if (_onChange) {
                    _onChange(*this, _value);
                } else {
                    child().layout(_bound.hsplit(((_bound.width - _bound.height) * _value) + _bound.height).car);
                    shouldRepaint(*this);
                }
            }
            e.accept();
        }

        ProxyNode<Slider>::bubble(e);
    }
};

Child slider(f64 value, OnChange<f64> onChange, Child child) {
    return makeStrong<Slider>(value, std::move(onChange), std::move(child));
}

// MARK: Color -----------------------------------------------------------------

Child color(Gfx::Color color, OnChange<Gfx::Color>) {
    return button(
        NOP,
        ButtonStyle::regular(),
        empty({32, 16}) |
            box({
                .margin = 4,
                .borderRadius = 2,
                .borderWidth = 1,
                .borderPaint = Gfx::GRAY50.withOpacity(0.1),
                .backgroundPaint = color,
            }) |
            Ui::center() | Ui::bound()
    );
}

// MARK: Intent ----------------------------------------------------------------

struct Intent : public ProxyNode<Intent> {
    Func<void(Node &, Sys::Event &e)> _map;

    Intent(Func<void(Node &, Sys::Event &e)> map, Child child)
        : ProxyNode<Intent>(std::move(child)), _map(std::move(map)) {}

    void reconcile(Intent &o) override {
        _map = std::move(o._map);
        ProxyNode<Intent>::reconcile(o);
    }

    void event(Sys::Event &e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::event(e);
    }

    void bubble(Sys::Event &e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::bubble(e);
    }
};

Child intent(Func<void(Node &, Sys::Event &e)> map, Child child) {
    return makeStrong<Intent>(std::move(map), std::move(child));
}

} // namespace Karm::Ui

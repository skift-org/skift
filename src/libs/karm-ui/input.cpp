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
            .borderRadii = RADIUS,
            .backgroundFill = ramp[8],
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = ramp[7],
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = ramp[7],
            .backgroundFill = ramp[8],
        },
    };
}

ButtonStyle ButtonStyle::secondary() {
    return {
        .idleStyle = {
            .borderRadii = RADIUS,
            .backgroundFill = GRAY900,
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = GRAY800,
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = GRAY800,
            .backgroundFill = GRAY900,
        },
    };
}

ButtonStyle ButtonStyle::primary() {
    return {
        .idleStyle = {
            .borderRadii = RADIUS,
            .backgroundFill = ACCENT700,
            .foregroundFill = Gfx::WHITE,
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = ACCENT600,
            .foregroundFill = Gfx::WHITE,
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = ACCENT600,
            .backgroundFill = ACCENT700,
            .foregroundFill = Gfx::WHITE,
        },
    };
}

ButtonStyle ButtonStyle::outline() {
    return {
        .idleStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = GRAY800,
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = GRAY700,
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = GRAY700,
            .backgroundFill = GRAY800,
        },
    };
}

ButtonStyle ButtonStyle::subtle() {
    return {
        .idleStyle = {
            .foregroundFill = GRAY300,
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = GRAY700,
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = GRAY700,
            .backgroundFill = GRAY800,
        },
    };
}

ButtonStyle ButtonStyle::text() {
    return {
        .idleStyle = {
            .foregroundFill = GRAY300,
        },
        .pressStyle = {
            .foregroundFill = GRAY300,
        },
    };
}

ButtonStyle ButtonStyle::destructive() {
    return {
        .idleStyle = {
            .borderRadii = RADIUS,
            .foregroundFill = Gfx::RED500,
        },
        .hoverStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .backgroundFill = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadii = RADIUS,
            .borderWidth = 1,
            .borderFill = Gfx::RED600,
            .backgroundFill = Gfx::RED700,
        },
    };
}

ButtonStyle ButtonStyle::withRadii(Math::Radiif radii) const {
    return {
        idleStyle.withRadii(radii),
        hoverStyle.withRadii(radii),
        pressStyle.withRadii(radii),
    };
}

ButtonStyle ButtonStyle::withForegroundFill(Gfx::Fill fill) const {
    return {
        idleStyle.withForegroundFill(fill),
        hoverStyle.withForegroundFill(fill),
        pressStyle.withForegroundFill(fill),
    };
}

ButtonStyle ButtonStyle::withPadding(Math::Insetsi insets) const {
    return {
        idleStyle.withPadding(insets),
        hoverStyle.withPadding(insets),
        pressStyle.withPadding(insets),
    };
}

ButtonStyle ButtonStyle::withMargin(Math::Insetsi insets) const {
    return {
        idleStyle.withMargin(insets),
        hoverStyle.withMargin(insets),
        pressStyle.withMargin(insets),
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

        if (not _onPress) {
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

    void event(App::Event &e) override {
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
           insets({16, 6}) |
           center() |
           minSize({UNCONSTRAINED, 36}) |
           button(std::move(onPress), style);
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i) {
    return icon(i) |
           insets({6, 6}) |
           center() |
           minSize({36, 36}) |
           button(std::move(onPress), style);
}

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t) {
    return hflow(8, Math::Align::CENTER, icon(i), text(t)) |
           insets({12, 6, 16, 6}) |
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
    Text::ProseStyle _style;

    Strong<Text::Model> _model;
    OnChange<Text::Action> _onChange;

    Opt<Text::Prose> _text;

    Input(Text::ProseStyle style, Strong<Text::Model> model, OnChange<Text::Action> onChange)
        : _style(style), _model(model), _onChange(std::move(onChange)) {}

    void reconcile(Input &o) override {
        _style = o._style;
        _model = o._model;
        _onChange = std::move(o._onChange);

        // NOTE: The model might have changed,
        //       so we need to invalidate the presentation.
        _text = NONE;
    }

    Text::Prose &_ensureText() {
        if (not _text) {
            _text = Text::Prose(_style);
            _text->append(_model->runes());
        }
        return *_text;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(bound());
        g.origin(bound().xy);

        auto &text = _ensureText();

        text.paintCaret(g, _model->_cur.head, Ui::ACCENT500);
        text.paint(g);

        g.restore();
        if (debugShowLayoutBounds)
            g.plot(bound(), Gfx::CYAN);
    }

    void event(App::Event &e) override {
        auto a = Text::Action::fromEvent(e);
        if (a) {
            e.accept();
            _onChange(*this, *a);
        }
    }

    void layout(Math::Recti bound) override {
        _ensureText().layout(bound.width);
        View<Input>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _ensureText().layout(s.width);
        return size.ceil().cast<isize>();
    }
};

Child input(Text::ProseStyle style, Strong<Text::Model> text, OnChange<Text::Action> onChange) {
    return makeStrong<Input>(style, text, std::move(onChange));
}

Child input(Strong<Text::Model> text, OnChange<Text::Action> onChange) {
    return makeStrong<Input>(TextStyles::bodyMedium(), text, std::move(onChange));
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

    void bubble(App::Event &e) override {
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

// MARK: Intent ----------------------------------------------------------------

struct Intent : public ProxyNode<Intent> {
    Func<void(Node &, App::Event &e)> _map;

    Intent(Func<void(Node &, App::Event &e)> map, Child child)
        : ProxyNode<Intent>(std::move(child)), _map(std::move(map)) {}

    void reconcile(Intent &o) override {
        _map = std::move(o._map);
        ProxyNode<Intent>::reconcile(o);
    }

    void event(App::Event &e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::event(e);
    }

    void bubble(App::Event &e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::bubble(e);
    }
};

Child intent(Func<void(Node &, App::Event &e)> map, Child child) {
    return makeStrong<Intent>(std::move(map), std::move(child));
}

} // namespace Karm::Ui

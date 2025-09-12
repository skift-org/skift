module;

#include <karm-gfx/canvas.h>
#include <karm-gfx/icon.h>
#include <karm-gfx/prose.h>
#include <karm-math/align.h>

export module Karm.Ui:input;

import :drag;
import :focus;
import :funcs;
import :layout;
import :atoms;
import :view;
import :text;

namespace Karm::Ui {

// MARK: Button ----------------------------------------------------------------

export struct MouseListener {
    enum MouseState {
        IDLE,
        HOVER,
        PRESS,
    };

    MouseState _state = IDLE;
    Math::Vec2i _pos = {0, 0};

    auto state() const {
        return _state;
    }

    bool isIdle() const {
        return _state == IDLE;
    }

    bool isHover() const {
        return _state == HOVER;
    }

    bool isPress() const {
        return _state == PRESS;
    }

    auto pos() const {
        return _pos;
    }

    bool listen(Node& node, App::Event& event) {
        bool result = false;
        MouseState state = _state;

        if (auto e = event.is<App::MouseEvent>()) {
            if (not node.bound().contains(e->pos)) {
                state = IDLE;
            } else {
                if (state != PRESS) {
                    state = HOVER;
                }

                _pos = e->pos - node.bound().topStart();

                if (e->type == App::MouseEvent::PRESS and
                    e->button == App::MouseButton::LEFT) {
                    state = PRESS;
                    event.accept();

                } else if (e->type == App::MouseEvent::RELEASE and
                           e->button == App::MouseButton::LEFT) {
                    if (state == PRESS) {
                        state = HOVER;
                        result = true;
                        event.accept();
                    }
                }
            }
        } else if (auto e = event.is<App::MouseLeaveEvent>()) {
            state = IDLE;
        }

        if (state != _state) {
            _state = state;
            shouldRepaint(node);
        }

        return result;
    }
};

export struct ButtonStyle {
    static constexpr isize RADIUS = 4;

    BoxStyle idleStyle{};
    BoxStyle hoverStyle{};
    BoxStyle pressStyle{};
    BoxStyle disabledStyle = {
        .foregroundFill = GRAY600,
    };

    static ButtonStyle none() {
        return {};
    }

    static ButtonStyle regular(Gfx::ColorRamp ramp = GRAYS) {
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

    static ButtonStyle secondary() {
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

    static ButtonStyle primary() {
        return {
            .idleStyle = {
                .borderRadii = RADIUS,
                .backgroundFill = ACCENT500,
                .foregroundFill = Gfx::WHITE,
            },
            .hoverStyle = {
                .borderRadii = RADIUS,
                .borderWidth = 1,
                .backgroundFill = ACCENT400,
                .foregroundFill = Gfx::WHITE,
            },
            .pressStyle = {
                .borderRadii = RADIUS,
                .borderWidth = 1,
                .borderFill = ACCENT400,
                .backgroundFill = ACCENT500,
                .foregroundFill = Gfx::WHITE,
            },
        };
    }

    static ButtonStyle outline() {
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

    static ButtonStyle subtle() {
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

    static ButtonStyle text() {
        return {
            .idleStyle = {
                .foregroundFill = GRAY300,
            },
            .pressStyle = {
                .foregroundFill = GRAY300,
            },
        };
    }

    static ButtonStyle destructive() {
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

    ButtonStyle withRadii(Math::Radiif radii) const {
        return {
            idleStyle.withRadii(radii),
            hoverStyle.withRadii(radii),
            pressStyle.withRadii(radii),
        };
    }

    ButtonStyle withForegroundFill(Gfx::Fill fill) const {
        return {
            idleStyle.withForegroundFill(fill),
            hoverStyle.withForegroundFill(fill),
            pressStyle.withForegroundFill(fill),
        };
    }

    ButtonStyle withPadding(Math::Insetsi insets) const {
        return {
            idleStyle.withPadding(insets),
            hoverStyle.withPadding(insets),
            pressStyle.withPadding(insets),
        };
    }

    ButtonStyle withMargin(Math::Insetsi insets) const {
        return {
            idleStyle.withMargin(insets),
            hoverStyle.withMargin(insets),
            pressStyle.withMargin(insets),
        };
    }
};

struct Button : _Box<Button> {
    Opt<Send<>> _onPress;
    ButtonStyle _buttonStyle = ButtonStyle::regular();
    MouseListener _mouseListener;

    Button(Opt<Send<>> onPress, ButtonStyle style, Child child)
        : _Box<Button>(child),
          _onPress(std::move(onPress)),
          _buttonStyle(style) {}

    void reconcile(Button& o) override {
        _buttonStyle = o._buttonStyle;
        _onPress = std::move(o._onPress);

        if (not _onPress) {
            // Reset the mouse listener if the button is disabled.
            _mouseListener = {};
        }

        _Box<Button>::reconcile(o);
    }

    BoxStyle& boxStyle() override {
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

    void event(App::Event& e) override {
        _Box<Button>::event(e);
        if (e.accepted())
            return;

        if (_onPress and (_mouseListener.listen(*this, e) or e.is<KeyboardShortcutActivated>())) {
            _onPress(*this);
        }
    };
};

export Child button(Opt<Send<>> onPress, ButtonStyle style, Child child) {
    return makeRc<Button>(std::move(onPress), style, child);
}

export auto button(Opt<Send<>> onPress, ButtonStyle style) {
    return [onPress = std::move(onPress), style](Child child) mutable {
        return button(std::move(onPress), style, child);
    };
}

export Child button(Opt<Send<>> onPress, ButtonStyle style, Str t) {
    return text(t) |
           insets({6, 16}) |
           center() |
           minSize({UNCONSTRAINED, 36}) |
           button(std::move(onPress), style);
}

export Child button(Opt<Send<>> onPress, ButtonStyle style, Gfx::Icon i) {
    return icon(i) |
           insets(6) |
           center() |
           minSize({36, 36}) |
           button(std::move(onPress), style);
}

export Child button(Opt<Send<>> onPress, ButtonStyle style, Gfx::Icon i, Str t) {
    return hflow(8, Math::Align::CENTER, icon(i), text(t)) |
           insets({6, 16, 6, 12}) |
           minSize({UNCONSTRAINED, 36}) |
           button(std::move(onPress), style);
}

export Child button(Opt<Send<>> onPress, Child child) {
    return button(std::move(onPress), ButtonStyle::regular(), child);
}

export auto button(Opt<Send<>> onPress) {
    return [onPress = std::move(onPress)](Child child) mutable {
        return button(std::move(onPress), child);
    };
}

export Child button(Opt<Send<>> onPress, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), t);
}

export Child button(Opt<Send<>> onPress, Gfx::Icon i) {
    return button(std::move(onPress), ButtonStyle::regular(), i);
}

export Child button(Opt<Send<>> onPress, Gfx::Icon i, Str t) {
    return button(std::move(onPress), ButtonStyle::regular(), i, t);
}

// MARK: Input -----------------------------------------------------------------

struct Input : View<Input> {
    Gfx::ProseStyle _style;

    FocusListener _focus;
    Rc<TextModel> _model;
    Send<TextAction> _onChange;

    Opt<Rc<Gfx::Prose>> _text;

    Input(Gfx::ProseStyle style, Rc<TextModel> model, Send<TextAction> onChange)
        : _style(style), _model(model), _onChange(std::move(onChange)) {}

    void reconcile(Input& o) override {
        _style = o._style;
        _model = o._model;
        _onChange = std::move(o._onChange);

        // NOTE: The model might have changed,
        //       so we need to invalidate the presentation.
        _text = NONE;
    }

    Gfx::Prose& _ensureText() {
        if (not _text) {
            _text = makeRc<Gfx::Prose>(_style);
            (*_text)->append(_model->runes());
        }
        return **_text;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(bound());
        g.origin(bound().xy.cast<f64>());

        auto& text = _ensureText();

        if (_focus) {
            g.push();
            text.paintSelection(g, _model->_cur.head, _model->_cur.tail, Ui::ACCENT500.withOpacity(0.5));
            text.paintCaret(g, _model->_cur.head, _style.color.unwrapOr(Ui::GRAY100));
            g.pop();
        }

        g.fill(text);

        g.pop();
    }

    void event(App::Event& e) override {
        auto a = TextAction::fromEvent(e);
        if (a) {
            e.accept();
            _onChange(*this, *a);
        }
    }

    void layout(Math::Recti bound) override {
        _ensureText().layout(Au{bound.width});
        View<Input>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _ensureText().layout(Au{s.width});
        return size.ceil().cast<isize>();
    }
};

export Child input(Gfx::ProseStyle style, Rc<TextModel> text, Send<TextAction> onChange) {
    return makeRc<Input>(style, text, std::move(onChange));
}

export Child input(Rc<TextModel> text, Send<TextAction> onChange) {
    return makeRc<Input>(TextStyles::bodyMedium(), text, std::move(onChange));
}

struct SimpleInput : View<SimpleInput> {
    Gfx::ProseStyle _style;
    String _text;
    Send<String> _onChange;

    FocusListener _focus;
    Opt<TextModel> _model;
    Opt<Rc<Gfx::Prose>> _prose;

    SimpleInput(Gfx::ProseStyle style, String text, Send<String> onChange)
        : _style(style),
          _text(text),
          _onChange(std::move(onChange)) {}

    void reconcile(SimpleInput& o) override {
        _style = o._style;
        _onChange = std::move(o._onChange);
        if (_text != o._text) {
            _text = o._text;
            _model = NONE;
            _prose = NONE;
        }

        // NOTE: The model might have changed,
        //       so we need to invalidate the presentation.
        _prose = NONE;
    }

    TextModel& _ensureModel() {
        if (not _model)
            _model = TextModel(_text);
        return *_model;
    }

    Gfx::Prose& _ensureText() {
        if (not _prose) {
            _prose = makeRc<Gfx::Prose>(_style);
            (*_prose)->append(_ensureModel().runes());
        }
        return **_prose;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(bound());
        g.origin(bound().xy.cast<f64>());

        auto& text = _ensureText();

        if (_focus) {
            g.push();
            text.paintSelection(g, _model->_cur.head, _model->_cur.tail, Ui::ACCENT500.withOpacity(0.5));
            text.paintCaret(g, _ensureModel()._cur.head, _style.color.unwrapOr(Ui::GRAY100));
            g.pop();
        }

        g.fill(text);

        g.pop();
    }

    void event(App::Event& e) override {
        _focus.event(*this, e);
        auto a = TextAction::fromEvent(e);
        if (a) {
            e.accept();
            _ensureModel().reduce(*a);
            _text = _ensureModel().string();
            _prose = NONE;
            _onChange(*this, _text);
        }
    }

    void layout(Math::Recti bound) override {
        _ensureText().layout(Au{bound.width});
        View<SimpleInput>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _ensureText().layout(Au{s.width});
        return size.ceil().cast<isize>();
    }
};

export Child input(Gfx::ProseStyle style, String text, Send<String> onChange) {
    return makeRc<SimpleInput>(style, text, std::move(onChange));
}

// MARK: Slider ----------------------------------------------------------------

struct Slider : ProxyNode<Slider> {
    f64 _value = 0.0f;
    Send<f64> _onChange;
    Math::Recti _bound;

    Slider(f64 value, Send<f64> onChange, Child child)
        : ProxyNode<Slider>(std::move(child)),
          _value(value),
          _onChange(std::move(onChange)) {
    }

    void reconcile(Slider& o) override {
        _value = o._value;
        _onChange = o._onChange;

        ProxyNode<Slider>::reconcile(o);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        child().layout(_bound.hsplit(((r.width - r.height) * _value) + r.height).v0);
    }

    Math::Recti bound() override {
        return _bound;
    }

    void bubble(App::Event& e) override {
        if (auto dv = e.is<DragEvent>()) {
            if (dv->type == DragEvent::DRAG) {
                auto max = bound().width - bound().height;
                auto value = max * _value;
                value = clamp(value + dv->delta.x, 0.0f, max);
                _value = value / max;
                _onChange(*this, _value);
            }
            e.accept();
        }

        ProxyNode<Slider>::bubble(e);
    }
};

export Child slider(f64 value, Send<f64> onChange, Child child) {
    return makeRc<Slider>(value, std::move(onChange), std::move(child));
}

export auto slider(f64 value, Send<f64> onChange) {
    return [value, onChange = std::move(onChange)](Child child) mutable {
        return slider(value, std::move(onChange), std::move(child));
    };
}

} // namespace Karm::Ui

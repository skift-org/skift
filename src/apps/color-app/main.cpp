#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

namespace ColorPicker {

struct HsvPicker : public Ui::View<HsvPicker> {
    Gfx::Hsv _value;
    Ui::OnChange<Gfx::Hsv> _onChange;
    Ui::MouseListener _mouseListener;

    HsvPicker(Gfx::Hsv value, Ui::OnChange<Gfx::Hsv> onChange)
        : _value{value}, _onChange{std::move(onChange)} {}

    void reconcile(HsvPicker &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    Gfx::Hsv sampleHsv(Math::Vec2i pos) {
        return {
            _value.hue,
            (pos.x / (f64)bound().width),
            1 - (pos.y / (f64)bound().height),
        };
    }

    Gfx::Color sampleColor(Math::Vec2i pos) {
        return Gfx::hsvToRgb(sampleHsv(pos));
    }

    Math::Vec2i coordinates(Gfx::Hsv hsv) {
        return Math::Vec2i(
            bound().x + (hsv.saturation * bound().width),
            bound().y + ((1 - hsv.value) * bound().height));
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(bound());

        for (isize y = 0; y < bound().height; y++) {
            for (isize x = 0; x < bound().width; x++) {
                g.debugPlot({bound().x + x, bound().y + y}, sampleColor({x, y}));
            }
        }

        auto pos = coordinates(_value);

        g.fillStyle(Gfx::WHITE);
        g.fill(Math::Ellipsei{pos, 8});

        g.strokeStyle(Gfx::stroke(Gfx::BLACK.withOpacity(0.25)).withWidth(1).withAlign(Gfx::OUTSIDE_ALIGN));
        g.stroke();

        g.fillStyle(Gfx::hsvToRgb(_value));
        g.fill(Math::Ellipsei{pos, 6});

        g.restore();
    }

    void event(Events::Event &e) override {
        _mouseListener.listen(*this, e);

        if (_mouseListener.isPress() and e.is<Events::MouseEvent>()) {
            _value = sampleHsv(_mouseListener.pos());
            if (_onChange) {
                _onChange(*this, _value);
            }

            Ui::shouldRepaint(*this);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {256, 256};
    }
};

Ui::Child hsvPicker(Gfx::Hsv value, Ui::OnChange<Gfx::Hsv> onChange) {
    return makeStrong<HsvPicker>(value, std::move(onChange));
}

Ui::Child hsvPicker(Ui::State<Gfx::Hsv> state) {
    return hsvPicker(state.value(), [state](auto &, auto value) mutable {
        state.update(value);
    });
}

Ui::Child valueSlider(Ui::State<Gfx::Hsv> state) {
    auto value0 = state.value();
    value0.value = 0;

    auto value1 = state.value();
    value1.value = 1;

    return Ui::sliderRow(
        Ui::SliderStyle::gradiant(
            Gfx::hsvToRgb(value0),
            Gfx::hsvToRgb(value1)),
        state.value().value,
        [=](auto &, auto v) mutable {
            auto hsv = state.value();
            hsv.value = v;
            state.update(hsv);
        },
        "Value");
}

Ui::Child saturationSlider(Ui::State<Gfx::Hsv> state) {
    auto saturation0 = state.value();
    saturation0.saturation = 0;

    auto saturation1 = state.value();
    saturation1.saturation = 1;

    return Ui::sliderRow(
        Ui::SliderStyle::gradiant(
            Gfx::hsvToRgb(saturation0),
            Gfx::hsvToRgb(saturation1)),
        state.value().saturation,
        [=](auto &, auto v) mutable {
            auto hsv = state.value();
            hsv.saturation = v;
            state.update(hsv);
        },
        "Saturation");
}

Ui::Child hueSlider(Ui::State<Gfx::Hsv> state) {
    return Ui::sliderRow(
        Ui::SliderStyle::hsv(),
        state.value().hue / 360.0,
        [=](auto &, auto v) mutable {
            auto hsv = state.value();
            hsv.hue = v * 360;
            state.update(hsv);
        },
        "Hue");
}

Ui::Child hsvSliders(Ui::State<Gfx::Hsv> state) {
    return Ui::vflow(
        hueSlider(state),
        saturationSlider(state),
        valueSlider(state));
}

Ui::Child hsvPickerAndSliders(Ui::State<Gfx::Hsv> state) {
    return Ui::vflow(
        hsvPicker(state),
        hsvSliders(state));
}

Gfx::Color pickColor(Gfx::Color c) {
    if (c.luminance() > 0.7) {
        return Gfx::BLACK;
    }

    return Gfx::WHITE;
}

Ui::Child colorCell(Gfx::Color c, Ui::State<Gfx::Hsv> state) {
    auto cHsv = Gfx::rgbToHsv(c);

    Ui::BoxStyle boxStyle = {
        .borderRadius = 4,
        .borderWidth = 1,
        .borderPaint = Gfx::WHITE.withOpacity(0.1),
        .backgroundPaint = c,
    };

    return Ui::button(
        [=](auto &) mutable {
            state.update(cHsv);
        },
        Ui::box(
            boxStyle,
            Op::eq(state.value(), cHsv)
                ? Ui::icon(Mdi::CHECK, 32, pickColor(c))
                : Ui::empty(32)));
}

Ui::Child colorRamp(Gfx::ColorRamp ramp, Ui::State<Gfx::Hsv> state) {
    return Ui::hflow(
        4,
        iter(ramp)
            .map([=](Gfx::Color c) {
                return colorCell(c, state);
            })
            .collect<Ui::Children>());
}

Ui::Child colorRamps(Ui::State<Gfx::Hsv> state) {
    return Ui::maxSize(
        {Ui::UNCONSTRAINED, 256},
        Ui::vscroll(
            Ui::vflow(
                4,
                iter(Gfx::RAMPS)
                    .map([=](auto ramp) {
                        return colorRamp(ramp, state);
                    })
                    .collect<Ui::Children>())));
}

Ui::Child app() {
    return Ui::state(Gfx::Hsv{0, 0, 0}, [=](Ui::State<Gfx::Hsv> hsvState) mutable {
        return Ui::state(true, [=](Ui::State<bool> pageState) mutable {
            auto selector = Ui::hflow(
                4,
                Ui::button(
                    pageState.bindValue(true),
                    (pageState.value()) ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                    Mdi::EYEDROPPER, "Color Picker"),
                Ui::button(
                    pageState.bindValue(false),
                    (not pageState.value()) ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                    Mdi::PALETTE_SWATCH, "Palette"));

            return Ui::spacing(
                8,
                Ui::vflow(
                    8,
                    selector,
                    pageState.value()
                        ? Ui::grow(hsvPickerAndSliders(hsvState))
                        : Ui::grow(colorRamps(hsvState))));
        });
    });
}

} // namespace ColorPicker

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, ColorPicker::app());
}

#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/drag.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

#include "model.h"

namespace Hideo::Colors {

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

        g.strokeStyle(Gfx::stroke(Gfx::BLACK
                                      .withOpacity(0.25))
                          .withWidth(1)
                          .withAlign(Gfx::OUTSIDE_ALIGN));
        g.stroke();

        g.fillStyle(Gfx::hsvToRgb(_value));
        g.fill(Math::Ellipsei{pos, 6});

        g.restore();
    }

    void event(Sys::Event &e) override {
        _mouseListener.listen(*this, e);

        if (_mouseListener.isPress() and e.is<Events::MouseEvent>()) {
            _value = sampleHsv(_mouseListener.pos());
            if (_onChange)
                _onChange(*this, _value);

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

Ui::Child hsvPicker(State const &state) {
    return hsvPicker(state.hsv, [](auto &n, auto hsv) {
        Model::bubble(n, UpdateHsv{hsv});
    });
}

Ui::Child sliderThumb(Gfx::Color color) {
    return Ui::empty() |
           Ui::bound() |
           Ui::aspectRatio(1) |
           Ui::box({
               .borderRadius = 99,
               .borderWidth = 2,
               .borderPaint = Gfx::WHITE,
               .backgroundPaint = color,
           }) |
           Ui::box({
               .padding = 1,
               .borderRadius = 99,
               .borderWidth = 1,
               .borderPaint = Gfx::BLACK.withOpacity(0.25),
           }) |
           Ui::dragRegion();
}

Ui::Child valueSlider(State const &state) {
    auto hsv = state.hsv;
    auto background =
        Gfx::Gradient::hlinear()
            .withColors(
                Gfx::hsvToRgb(hsv.withValue(0)),
                Gfx::hsvToRgb(hsv.withValue(1)))
            .bake();

    return sliderThumb(Gfx::hsvToRgb(hsv)) |
           Ui::end() |
           Ui::slider(hsv.value, [hsv](auto &n, auto v) {
               Model::bubble(n, UpdateHsv{hsv.withValue(v)});
           }) |
           Ui::box({
               .padding = 3,
               .borderRadius = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 26});
}

Ui::Child saturationSlider(State const &state) {
    auto hsv = state.hsv;
    auto background =
        Gfx::Gradient::hlinear()
            .withColors(
                Gfx::hsvToRgb(
                    hsv
                        .withValue(1)
                        .withSaturation(0)),
                Gfx::hsvToRgb(
                    hsv
                        .withValue(1)
                        .withSaturation(1)))
            .bake();

    return sliderThumb(Gfx::hsvToRgb(hsv.withValue(1))) |
           Ui::end() |
           Ui::slider(hsv.saturation, [hsv](auto &n, auto v) {
               Model::bubble(n, UpdateHsv{hsv.withSaturation(v)});
           }) |
           Ui::box({
               .padding = 3,
               .borderRadius = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 26});
}

Ui::Child hueSlider(State const &state) {
    auto hsv = state.hsv;

    return sliderThumb(Gfx::hsvToRgb(hsv.withSaturation(1).withValue(1))) |
           Ui::end() |
           Ui::slider(hsv.hue / 360, [hsv](auto &n, auto v) {
               Model::bubble(n, UpdateHsv{hsv.withHue(v * 360)});
           }) |
           Ui::box({
               .padding = 3,
               .borderRadius = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = Gfx::Gradient::hsv().bake(),
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 26});
}

Ui::Child hsvSliders(State const &state) {
    return Ui::vflow(
        8,
        hueSlider(state),
        saturationSlider(state),
        valueSlider(state));
}

Gfx::Color pickColor(Gfx::Color c) {
    if (c.luminance() > 0.7)
        return Gfx::BLACK;
    return Gfx::WHITE;
}

Ui::Child colorCell(State const &state, Gfx::Color c) {
    auto hsv = Gfx::rgbToHsv(c);

    Ui::ButtonStyle style = Ui::ButtonStyle::primary();
    auto foreground = pickColor(c);

    style.idleStyle.backgroundPaint = Gfx::Paint{c};
    style.idleStyle.borderPaint = Gfx::Paint{foreground.withOpacity(0.1)};
    style.idleStyle.borderWidth = 1;

    style.hoverStyle.backgroundPaint = Gfx::Paint{foreground.withOpacity(0.2).blendOver(c)};

    style.pressStyle.backgroundPaint = Gfx::Paint{foreground.withOpacity(0.01).blendOver(c)};
    style.pressStyle.borderPaint = Gfx::Paint{foreground.withOpacity(0.1)};

    return Ui::button(
        Model::bind<UpdateHsv>(hsv),
        style,
        state.hsv == hsv
            ? Ui::icon(Mdi::CHECK, 32, foreground) | Ui::center() | Ui::bound()
            : Ui::empty(32));
}

Ui::Child colorRamp(State const &state, Gfx::ColorRamp ramp) {
    return Ui::hflow(
        4,
        iter(ramp)
            .map([=](Gfx::Color c) {
                return colorCell(state, c) | Ui::grow();
            })
            .collect<Ui::Children>());
}

Ui::Child colorRamps(State const &state) {
    return Ui::vflow(
               4,
               iter(Gfx::RAMPS)
                   .map([=](auto ramp) {
                       return colorRamp(state, ramp);
                   })
                   .collect<Ui::Children>()) |
           Ui::spacing({0, 0, 0, 8}) |
           Ui::vscroll() |
           Ui::maxSize({Ui::UNCONSTRAINED, 256});
}

Ui::Child app() {
    return Ui::reducer<Model>(
        {
            .hsv = Gfx::rgbToHsv(Gfx::BLUE),
        },
        [](auto const &state) {
            auto c = Gfx::hsvToRgb(state.hsv);

            auto preview = Ui::empty(128) |
                           Ui::box({
                               .borderRadius = 6,
                               .borderWidth = 1,
                               .borderPaint = Ui::GRAY100.withOpacity(0.1),
                               .backgroundPaint = Gfx::hsvToRgb(state.hsv),
                           });

            auto toolbar = Ui::hflow(
                4,
                Ui::empty(4),
                Ui::codeLarge("#{:02x}{:02x}{:02x}", c.red, c.green, c.blue) | Ui::vcenter(),
                Ui::button(
                    Ui::NOP,
                    Ui::ButtonStyle::subtle(),
                    Mdi::CONTENT_COPY),
                Ui::grow(NONE),
                Ui::button(
                    Model::bind(UpdatePage{Page::HSV}),
                    (state.page == Page::HSV) ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                    Mdi::TUNE_VARIANT),
                Ui::button(
                    Model::bind(UpdatePage{Page::PALLETE}),
                    (state.page == Page::PALLETE) ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                    Mdi::PALETTE_SWATCH));

            return Ui::vflow(
                       8,
                       preview,
                       toolbar,
                       Ui::titleMedium("Values") | Ui::spacing({2, 8, 0, 0}),
                       hsvSliders(state),
                       Ui::titleMedium("Palettes") | Ui::spacing({2, 8, 0, 0}),
                       colorRamps(state) | Ui::grow()) |
                   Ui::spacing({8, 8, 8, 0}) | Ui::grow();
        });
}

} // namespace Hideo::Colors

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Colors::app() | Ui::pinSize({410, 520}));
}

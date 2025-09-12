module;

#include <karm-gfx/canvas.h>
#include <karm-gfx/shadow.h>
#include <karm-math/align.h>

export module Karm.Kira:colorInput;

import Karm.App;
import Karm.Ui;
import :dialog;

namespace Karm::Kira {

// MARK: Hsv Input -------------------------------------------------------------

struct HsvSquare : Ui::View<HsvSquare> {
    Gfx::Hsv _value;
    Ui::Send<Gfx::Hsv> _onChange;
    Ui::MouseListener _mouseListener;

    HsvSquare(Gfx::Hsv value, Ui::Send<Gfx::Hsv> onChange)
        : _value{value}, _onChange{std::move(onChange)} {}

    void reconcile(HsvSquare& o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    auto makeHsvSquare() {
        auto surf = Gfx::Surface::alloc({256, 256});

        for (isize y = 0; y < surf->height(); y++) {
            for (isize x = 0; x < surf->width(); x++) {
                surf->mutPixels().store(
                    {x, y},
                    Gfx::hsvToRgb({
                        _value.hue,
                        (x / (f64)surf->width()),
                        1 - (y / (f64)surf->height()),
                    })
                );
            }
        }

        return surf;
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

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(bound());

        auto hsv = makeHsvSquare();

        g.blit(bound(), *hsv);

        Math::Vec2i pos = {
            bound().x + (isize)(_value.saturation * bound().width),
            bound().y + (isize)((1 - _value.value) * bound().height),
        };

        g.fillStyle(Gfx::WHITE);
        g.fill(Math::Ellipsef{pos.cast<f64>(), 8.});

        g.strokeStyle(
            Gfx::stroke(
                Gfx::BLACK.withOpacity(0.25)
            )
                .withWidth(1)
                .withAlign(Gfx::OUTSIDE_ALIGN)
        );
        g.stroke();

        g.fillStyle(Gfx::hsvToRgb(_value));
        g.fill(Math::Ellipsef{pos.cast<f64>(), 6});

        g.pop();
    }

    void event(App::Event& e) override {
        _mouseListener.listen(*this, e);

        if (_mouseListener.isPress() and e.is<App::MouseEvent>()) {
            _value = sampleHsv(_mouseListener.pos());
            _onChange(*this, _value);
            Ui::shouldRepaint(*this);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {256, 256};
    }
};

export Ui::Child hsvSquare(Gfx::Hsv value, Ui::Send<Gfx::Hsv> onChange) {
    return makeRc<HsvSquare>(value, std::move(onChange));
}

static Ui::Child _sliderThumb(Gfx::Color color) {
    return Ui::empty() |
           Ui::bound() |
           Ui::aspectRatio(1) |
           Ui::box({
               .borderRadii = 99,
               .borderWidth = 2,
               .borderFill = Gfx::WHITE,
               .backgroundFill = color,
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderFill = Gfx::BLACK.withOpacity(0.25),
           }) |
           Ui::dragRegion();
}

export Ui::Child hsvValueSlider(Gfx::Hsv hsv, Ui::Send<Gfx::Hsv> onChange) {
    auto background =
        Gfx::Gradient::hlinear()
            .withColors(
                Gfx::hsvToRgb(hsv.withValue(0)),
                Gfx::hsvToRgb(hsv.withValue(1))
            )
            .bake();

    return _sliderThumb(Gfx::hsvToRgb(hsv)) |
           Ui::end() |
           Ui::slider(hsv.value, [hsv, onChange](auto& n, auto v) {
               onChange(n, hsv.withValue(v));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderFill = Ui::GRAY100.withOpacity(0.2),
               .backgroundFill = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 18});
}

export Ui::Child hsvSaturationSlider(Gfx::Hsv hsv, Ui::Send<Gfx::Hsv> onChange) {
    auto background =
        Gfx::Gradient::hlinear()
            .withColors(
                Gfx::hsvToRgb(
                    hsv
                        .withValue(1)
                        .withSaturation(0)
                ),
                Gfx::hsvToRgb(
                    hsv
                        .withValue(1)
                        .withSaturation(1)
                )
            )
            .bake();

    return _sliderThumb(Gfx::hsvToRgb(hsv.withValue(1))) |
           Ui::end() |
           Ui::slider(hsv.saturation, [hsv, onChange](auto& n, auto v) {
               onChange(n, hsv.withSaturation(v));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderFill = Ui::GRAY100.withOpacity(0.2),
               .backgroundFill = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 18});
}

export Ui::Child hsvHueSlider(Gfx::Hsv hsv, Ui::Send<Gfx::Hsv> onChange) {
    return _sliderThumb(Gfx::hsvToRgb(hsv.withSaturation(1).withValue(1))) |
           Ui::end() |
           Ui::slider(hsv.hue / 360, [hsv, onChange](auto& n, auto v) {
               onChange(n, hsv.withHue(v * 360));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderFill = Ui::GRAY100.withOpacity(0.2),
               .backgroundFill = Gfx::Gradient::hsv().bake(),
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 18});
}

// MARK: Color Picker ----------------------------------------------------------

enum struct Page {
    HSV,
    PALLETE,
};

struct State {
    Page page = Page::HSV;
    Gfx::Hsv hsv;
};

struct UpdatePage {
    Page page;
};

struct UpdateHsv {
    Gfx::Hsv hsv;

    UpdateHsv(Gfx::Hsv hsv) : hsv(hsv) {}
};

using Action = Union<UpdatePage, UpdateHsv>;

Ui::Task<Action> reduce(State& s, Action action) {
    action.visit(Visitor{
        [&](UpdatePage update) {
            s.page = update.page;
        },
        [&](UpdateHsv update) {
            s.hsv = update.hsv;
        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

Gfx::Color pickColor(Gfx::Color c) {
    if (c.luminance() > 0.7)
        return Gfx::BLACK;
    return Gfx::WHITE;
}

export Ui::Child colorPickerDialog() {
    return Ui::reducer<Model>(
        {
            .hsv = Gfx::rgbToHsv(Gfx::BLUE),
        },
        [](auto const& s) {
            auto c = Gfx::hsvToRgb(s.hsv);

            auto preview =
                Ui::codeLarge(pickColor(c), "#{:02x}{:02x}{:02x}", c.red, c.green, c.blue) |
                Ui::center() |
                Ui::minSize(96) |
                Ui::box({
                    .borderRadii = 6,
                    .borderWidth = 1,
                    .borderFill = Ui::GRAY100.withOpacity(0.1),
                    .backgroundFill = Gfx::hsvToRgb(s.hsv),
                    .shadowStyle = Gfx::BoxShadow::elevated(16, Gfx::hsvToRgb(s.hsv).withOpacity(0.5)),
                });

            auto content =
                Ui::vflow(
                    8,
                    preview,
                    hsvHueSlider(
                        s.hsv,
                        Model::map<UpdateHsv>()
                    ),
                    hsvSaturationSlider(
                        s.hsv,
                        Model::map<UpdateHsv>()
                    ),
                    hsvValueSlider(
                        s.hsv,
                        Model::map<UpdateHsv>()
                    )
                ) |
                Ui::minSize({256, Ui::UNCONSTRAINED});
            return dialogContent({
                dialogTitleBar("Color Picker"s),
                content | Ui::insets({8, 16}),
                dialogFooter({
                    dialogCancel(),
                    dialogAction(Ui::SINK<>, "Ok"s),
                }),
            });
        }
    );
}

// MARK: Color Input -----------------------------------------------------------

export Ui::Child colorInput(Gfx::Color color, Ui::Send<Gfx::Color>) {
    auto colorPreview =
        Ui::empty({18, 18}) |
        Ui::box({
            .margin = 4,
            .borderRadii = 2,
            .borderWidth = 1,
            .borderFill = Gfx::GRAY50.withOpacity(0.1),
            .backgroundFill = color,
        });

    auto hexPreview =
        Ui::codeMedium("#{:02x}{:02x}{:02x}", color.red, color.green, color.blue) | Ui::vcenter();

    return hflow(8, Math::Align::CENTER, colorPreview, hexPreview) |
           Ui::insets({6, 12, 6, 6}) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           button(
               [](auto& n) {
                   Ui::showDialog(n, colorPickerDialog());
               },
               Ui::ButtonStyle::outline()
           );
}

} // namespace Karm::Kira

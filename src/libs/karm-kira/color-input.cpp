#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>

#include "color-input.h"
#include "dialog.h"

namespace Karm::Kira {

// MARK: Hsv Input -------------------------------------------------------------

struct HsvSquare : public Ui::View<HsvSquare> {
    Gfx::Hsv _value;
    Ui::OnChange<Gfx::Hsv> _onChange;
    Ui::MouseListener _mouseListener;

    HsvSquare(Gfx::Hsv value, Ui::OnChange<Gfx::Hsv> onChange)
        : _value{value}, _onChange{std::move(onChange)} {}

    void reconcile(HsvSquare &o) override {
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
            bound().y + ((1 - hsv.value) * bound().height)
        );
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(bound());

        for (isize y = 0; y < bound().height; y++) {
            for (isize x = 0; x < bound().width; x++) {
                g._fillRect(Math::Recti{bound().x + x, bound().y + y, 1, 1}, sampleColor({x, y}));
            }
        }

        auto pos = coordinates(_value);

        g.fillStyle(Gfx::WHITE);
        g.fill(Math::Ellipsef{pos.cast<f64>(), 8.});

        g.strokeStyle(
            Gfx::stroke(Gfx::BLACK
                            .withOpacity(0.25))
                .withWidth(1)
                .withAlign(Gfx::OUTSIDE_ALIGN)
        );
        g.stroke();

        g.fillStyle(Gfx::hsvToRgb(_value));
        g.fill(Math::Ellipsef{pos.cast<f64>(), 6});

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

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {256, 256};
    }
};

Ui::Child hsvSquare(Gfx::Hsv value, Ui::OnChange<Gfx::Hsv> onChange) {
    return makeStrong<HsvSquare>(value, std::move(onChange));
}

static Ui::Child _sliderThumb(Gfx::Color color) {
    return Ui::empty() |
           Ui::bound() |
           Ui::aspectRatio(1) |
           Ui::box({
               .borderRadii = 99,
               .borderWidth = 2,
               .borderPaint = Gfx::WHITE,
               .backgroundPaint = color,
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderPaint = Gfx::BLACK.withOpacity(0.25),
           }) |
           Ui::dragRegion();
}

Ui::Child hsvValueSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange) {
    auto background =
        Gfx::Gradient::hlinear()
            .withColors(
                Gfx::hsvToRgb(hsv.withValue(0)),
                Gfx::hsvToRgb(hsv.withValue(1))
            )
            .bake();

    return _sliderThumb(Gfx::hsvToRgb(hsv)) |
           Ui::end() |
           Ui::slider(hsv.value, [hsv, onChange](auto &n, auto v) {
               onChange(n, hsv.withValue(v));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 18});
}

Ui::Child hsvSaturationSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange) {
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
           Ui::slider(hsv.saturation, [hsv, onChange](auto &n, auto v) {
               onChange(n, hsv.withSaturation(v));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = background,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 18});
}

Ui::Child hsvHueSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange) {
    return _sliderThumb(Gfx::hsvToRgb(hsv.withSaturation(1).withValue(1))) |
           Ui::end() |
           Ui::slider(hsv.hue / 360, [hsv, onChange](auto &n, auto v) {
               onChange(n, hsv.withHue(v * 360));
           }) |
           Ui::box({
               .padding = 1,
               .borderRadii = 99,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY100.withOpacity(0.2),
               .backgroundPaint = Gfx::Gradient::hsv().bake(),
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

void reduce(State &s, Action action) {
    action.visit(Visitor{
        [&](UpdatePage update) {
            s.page = update.page;
        },
        [&](UpdateHsv update) {
            s.hsv = update.hsv;
        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

Gfx::Color pickColor(Gfx::Color c) {
    if (c.luminance() > 0.7)
        return Gfx::BLACK;
    return Gfx::WHITE;
}

Ui::Child colorPickerDialog() {
    return Ui::reducer<Model>(
        {
            .hsv = Gfx::rgbToHsv(Gfx::BLUE),
        },
        [](auto const &s) {
            auto c = Gfx::hsvToRgb(s.hsv);

            auto preview =
                Ui::codeLarge(pickColor(c), "#{:02x}{:02x}{:02x}", c.red, c.green, c.blue) |
                Ui::center() |
                Ui::minSize(96) |
                Ui::box({
                    .borderRadii = 6,
                    .borderWidth = 1,
                    .borderPaint = Ui::GRAY100.withOpacity(0.1),
                    .backgroundPaint = Gfx::hsvToRgb(s.hsv),
                    .shadowStyle = Gfx::BoxShadow::elevated(16, Gfx::hsvToRgb(s.hsv).withOpacity(0.5)),
                });

            auto content =
                Ui::vflow(
                    8,
                    preview,
                    Kr::hsvHueSlider(
                        s.hsv,
                        [](auto &n, auto v) {
                            Model::bubble<UpdateHsv>(n, v);
                        }
                    ),
                    Kr::hsvSaturationSlider(
                        s.hsv,
                        [](auto &n, auto v) {
                            Model::bubble<UpdateHsv>(n, v);
                        }
                    ),
                    Kr::hsvValueSlider(
                        s.hsv,
                        [](auto &n, auto v) {
                            Model::bubble<UpdateHsv>(n, v);
                        }
                    )
                ) |
                Ui::minSize({256, Ui::UNCONSTRAINED});

            return Kr::dialogContent({

                Kr::dialogTitleBar("Color Picker"s),
                content |
                    Ui::spacing({16, 8}),
                Kr::dialogFooter(
                    {
                        Kr::dialogCancel(),
                        Kr::dialogAction(Ui::NOP, "Ok"s),
                    }
                )
            });
        }
    );
}

// MARK: Color Input -----------------------------------------------------------

Ui::Child colorInput(Gfx::Color color, Ui::OnChange<Gfx::Color>) {
    auto colorPreview =
        Ui::empty({18, 18}) |
        Ui::box({
            .margin = 4,
            .borderRadii = 2,
            .borderWidth = 1,
            .borderPaint = Gfx::GRAY50.withOpacity(0.1),
            .backgroundPaint = color,
        });

    auto hexPreview =
        Ui::codeMedium("#{:02x}{:02x}{:02x}", color.red, color.green, color.blue) | Ui::vcenter();

    return hflow(8, Math::Align::CENTER, colorPreview, hexPreview) |
           Ui::spacing({6, 6, 12, 6}) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           button(
               [](auto &n) {
                   Ui::showDialog(n, colorPickerDialog());
               },
               Ui::ButtonStyle::outline()
           );
}

} // namespace Karm::Kira

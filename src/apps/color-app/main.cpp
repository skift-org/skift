#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

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
            (pos.x / (double)bound().width),
            1 - (pos.y / (double)bound().height),
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

        for (int y = 0; y < bound().height; y++) {
            for (int x = 0; x < bound().width; x++) {
                g.plot({bound().x + x, bound().y + y}, sampleColor({x, y}));
            }
        }

        auto pos = coordinates(_value);

        g.fillStyle(Gfx::WHITE);
        g.fill(Math::Ellipsei{pos, 12});

        g.strokeStyle(Gfx::StrokeStyle{Gfx::BLACK.withOpacity(0.25)}.withWidth(1).withAlign(Gfx::OUTSIDE_ALIGN));
        g.stroke();

        g.fillStyle(Gfx::hsvToRgb(_value));
        g.fill(Math::Ellipsei{pos, 8});

        g.restore();
    }

    void event(Events::Event &e) override {
        _mouseListener.listen(*this, e);

        if (_mouseListener.isPress() && e.is<Events::MouseEvent>()) {
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

Gfx::Color pickColor(Gfx::Color c) {
    if (c.luminance() > 0.7) {
        return Gfx::BLACK;
    }

    return Gfx::WHITE;
}

Ui::Child colorCell(Gfx::Color c, Ui::State<Gfx::Hsv> state) {
    auto cHsv = Gfx::rgbToHsv(c);

    return Ui::button(
        [=](auto &) mutable {
            state.update(cHsv);
        },
        Ui::box(
            {
                .borderRadius = 4,
                .borderWidth = 1,
                .borderColor = Gfx::WHITE.withOpacity(0.1),
                .backgroundColor = c,
            },
            Op::eq(state.value(), cHsv) ? Ui::icon(Media::Icons::CHECK, 32, pickColor(c)) : Ui::empty(32)));
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

Ui::Child colorPicker() {
    return Ui::state(
        Gfx::Hsv{0, 0, 0},
        [=](Ui::State<Gfx::Hsv> state) mutable {
            return Ui::vflow(
                Ui::vflow(
                    Ui::titleRow("Color Picker"),
                    hsvPicker(
                        state.value(),
                        [=](auto &, auto v) mutable {
                            state.update(v);
                        }),
                    Ui::separator(),
                    Ui::titleRow("Color Palette"),
                    Ui::spacing(
                        {12, 0},
                        colorRamps(state))));
        });
}

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, colorPicker());
}

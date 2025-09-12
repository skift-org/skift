module;

#include <karm-gfx/canvas.h>
#include <karm-gfx/icon.h>

export module Hideo.Images:editor;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;
import :model;

namespace Hideo::Images {

Ui::Child histogram(Hist const& hist) {
    return Ui::canvas([hist](Gfx::Canvas& g, Math::Vec2i size) {
        f64 xunit = size.x / ((f64)hist.len() - 1);

        auto point = [&](usize i, usize c) {
            return Math::Vec2f{i * xunit, (f64)size.y - hist[i]._els[c] * size.y};
        };

        auto plotComponent = [&](usize c, Gfx::Color stroke, Gfx::Color fill) {
            g.beginPath();
            g.moveTo(point(0, c));
            for (usize i = 0; i < hist.len() - 1; i++) {
                auto p0 = point(i, c);
                auto p1 = point(i + 1, c);

                g.cubicTo(
                    p0 + Math::Vec2f{xunit / 4, 0},
                    p1 - Math::Vec2f{xunit / 4, 0},
                    p1
                );
            }
            g.stroke(Gfx::stroke(stroke));

            g.lineTo({0, (f64)size.y});
            g.closePath();

            g.fill(fill.withOpacity(0.25));
        };

        plotComponent(0, Gfx::RED, Gfx::RED800);
        plotComponent(1, Gfx::GREEN, Gfx::GREEN800);
        plotComponent(2, Gfx::BLUE, Gfx::BLUE800);
    });
}

Ui::Child editorPreview(State const& state) {
    return Ui::image(state.image.unwrap()) |
           Ui::box({
               .borderWidth = 1,
               .borderFill = Ui::GRAY50.withOpacity(0.1),
           }) |
           Ui::foregroundFilter(state.filter) |
           Ui::insets(8) |
           Ui::fit();
}

Ui::Child editorToolbar(State const&) {
    return Kr::toolbar({

        Ui::grow(NONE),

    });
}

Ui::Child editorFilterTile(Ui::Send<> onPress, Ui::ButtonStyle style, Gfx::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()
           ) |
           Ui::insets(8) |
           Ui::bound() |
           Ui::minSize({96, 72}) |
           Ui::button(onPress, style);
}

template <typename T>
Gfx::Icon editorFilterIcon() {
    if constexpr (Meta::Same<T, Gfx::Unfiltered>) {
        return Mdi::IMAGE;
    }

    if constexpr (Meta::Same<T, Gfx::BlurFilter>) {
        return Mdi::BLUR;
    }

    if constexpr (Meta::Contains<T, Gfx::SaturationFilter, Gfx::GrayscaleFilter>) {
        return Mdi::INVERT_COLORS;
    }

    if constexpr (Meta::Same<T, Gfx::ContrastFilter>) {
        return Mdi::CONTRAST_CIRCLE;
    }

    if constexpr (Meta::Same<T, Gfx::BrightnessFilter>) {
        return Mdi::LIGHTBULB;
    }

    if constexpr (Meta::Same<T, Gfx::SepiaFilter>) {
        return Mdi::IMAGE_FILTER_VINTAGE;
    }

    if constexpr (Meta::Same<T, Gfx::NoiseFilter>) {
        return Mdi::GRAIN;
    }

    if constexpr (Meta::Same<T, Gfx::TintFilter>) {
        return Mdi::EYEDROPPER;
    }

    if constexpr (Meta::Same<T, Gfx::OverlayFilter>) {
        return Mdi::EYEDROPPER_PLUS;
    }

    return Mdi::AUTO_FIX;
}

Ui::Child editorFilterControls(Gfx::Filter const& filter) {
    return filter.visit(Visitor{
        [](Gfx::Unfiltered const&) {
            return Ui::empty();
        },
        [](Gfx::TintFilter const&) {
            return Ui::empty();
        },
        [](Gfx::OverlayFilter const&) {
            return Ui::empty();
        },
        [](Gfx::GrayscaleFilter const&) {
            return Ui::empty();
        },
        []<typename T>(T const& f) {
            return Kr::slider<decltype(f.amount)>(
                f.amount,
                T::RANGE,
                [](auto& n, auto v) {
                    Model::bubble(n, SetFilter{T{v}});
                },
                Mdi::DRAG_VERTICAL_VARIANT,
                Io::toTitleCase(T::NAME).unwrap()
            );
        },
    });
}

Ui::Child editorFilters(State const& s) {
    Ui::Children tiles;
    Gfx::Filter::any([&]<typename T>() {
        tiles.pushBack(
            editorFilterTile(
                [](auto& n) {
                    Model::bubble(n, SetFilter{T{}});
                },
                s.filter.is<T>() ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                editorFilterIcon<T>(),
                Io::toTitleCase(T::NAME).unwrap()
            )
        );

        return false;
    });

    return Ui::hflow(8, tiles);
}

Ui::Child editorControls(State const& state) {
    return vflow(
               4,
               editorFilterControls(state.filter),
               editorFilters(state) |
                   Ui::insets({0, 8, 8, 8}) |
                   Ui::hscroll()
           ) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

Ui::Child editorProperties(State const& s) {
    return Ui::vflow(
        histogram(s.hist) | Ui::pinSize(64),
        Kr::separator(),
        Ui::vscroll(
            Ui::vflow(
                Kr::titleRow("Composition"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Rotation"s),
                Kr::separator(),

                Kr::titleRow("Light"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Brightness"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Exposure"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Gamma"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Contrast"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Shadow"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Highlights"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Bloom"s),
                Kr::separator(),

                Kr::titleRow("Color"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Temparature"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Tint"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Vibrance"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Saturation"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Sepia"s),
                Kr::separator(),

                Kr::titleRow("Effects"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Clarity"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Noise"s),
                Kr::sliderRow(0, Ui::SINK<f64>, "Vignette"s),
                Kr::separator(),

                Kr::titleRow("Curves"s),
                Kr::separator(),

                Kr::titleRow("Filters"s)
            )
        ) | Ui::grow()
    );
}

Ui::Child editorApp(State const& s) {
    return Kr::scaffold({
        .icon = Mdi::IMAGE,
        .title = "Images"s,
        .startTools = [&] -> Ui::Children {
            return {
                Ui::button(
                    Model::bind<ToggleEditor>(),
                    Ui::ButtonStyle::subtle(),
                    Mdi::UNDO
                ),

                Ui::button(
                    Model::bind<ToggleEditor>(),
                    Ui::ButtonStyle::subtle(),
                    Mdi::REDO
                ),
            };
        },
        .endTools = [&] -> Ui::Children {
            return {
                Ui::button(
                    Model::bind<ToggleEditor>(),
                    Ui::ButtonStyle::subtle(),
                    Mdi::CANCEL,
                    "Cancel"
                ),

                Ui::button(
                    Model::bind<SaveImage>(),
                    Ui::ButtonStyle::primary(),
                    Mdi::FLOPPY,
                    "Save Changes"
                ),
            };
        },
        .body = [&] {
            return Ui::hflow(
                editorPreview(s) | Ui::grow(),
                editorProperties(s) | Kr::resizable(Kr::ResizeHandle::START, {320}, NONE)
            );
        },
    });
}

} // namespace Hideo::Images

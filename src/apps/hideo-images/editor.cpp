#include <karm-kira/row.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/slider.h>
#include <karm-kira/toolbar.h>
#include <karm-ui/anim.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/auto-fix.h>
#include <mdi/blur.h>
#include <mdi/cancel.h>
#include <mdi/contrast-circle.h>
#include <mdi/drag-vertical-variant.h>
#include <mdi/eyedropper-plus.h>
#include <mdi/eyedropper.h>
#include <mdi/floppy.h>
#include <mdi/grain.h>
#include <mdi/image-filter-vintage.h>
#include <mdi/image.h>
#include <mdi/invert-colors.h>
#include <mdi/lightbulb.h>

#include "app.h"

namespace Hideo::Images {

Ui::Child histogram(Hist const &hist) {
    return Ui::canvas([hist](Gfx::Canvas &g, Math::Vec2i size) {
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

Ui::Child editor(State const &state) {
    return Ui::vflow(
        editorToolbar(state),
        histogram(state.hist) | Ui::pinSize(64),
        editorPreview(state) | Ui::grow(),
        editorControls(state)
    );
}

Ui::Child editorPreview(State const &state) {
    return Ui::image(state.image.unwrap()) |
           Ui::box({
               .borderWidth = 1,
               .borderFill = Ui::GRAY50.withOpacity(0.1),
           }) |
           Ui::foregroundFilter(state.filter) |
           Ui::insets(8) |
           Ui::fit();
}

Ui::Child editorToolbar(State const &) {
    return Kr::toolbar({
        Ui::grow(NONE),

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
    });
}

Ui::Child editorFilterTile(Ui::OnPress onPress, Ui::ButtonStyle style, Mdi::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()
           ) |
           Ui::insets(8) |
           Ui::bound() |
           Ui::minSize({96, 72}) |
           Ui::button(std::move(onPress), style);
}

template <typename T>
Mdi::Icon editorFilterIcon() {
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

Ui::Child editorFilterControls(Gfx::Filter const &filter) {
    return filter.visit(Visitor{
        [](Gfx::Unfiltered const &) {
            return Ui::empty();
        },
        [](Gfx::TintFilter const &) {
            return Ui::empty();
        },
        [](Gfx::OverlayFilter const &) {
            return Ui::empty();
        },
        [](Gfx::GrayscaleFilter const &) {
            return Ui::empty();
        },
        []<typename T>(T const &f) {
            return Kr::slider<decltype(f.amount)>(
                f.amount,
                T::RANGE,
                [](auto &n, auto v) {
                    Model::bubble(n, SetFilter{T{v}});
                },
                Mdi::DRAG_VERTICAL_VARIANT,
                Io::toTitleCase(T::NAME).unwrap()
            );
        }
    });
}

Ui::Child editorFilters(State const &s) {
    Ui::Children tiles;
    Gfx::Filter::any([&]<typename T>(Meta::Type<T>) {
        tiles.pushBack(
            editorFilterTile(
                [](auto &n) {
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

Ui::Child editorControls(State const &state) {
    return vflow(
               4,
               editorFilterControls(state.filter),
               editorFilters(state) |
                   Ui::insets({0, 8, 8, 8}) |
                   Ui::hscroll()
           ) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

} // namespace Hideo::Images

#include <karm-fmt/case.h>
#include <karm-ui/input.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace ImageViewer {

Ui::Child editor(State const &state) {
    return Ui::vflow(
        editorToolbar(state),
        editorPreview(state) | Ui::grow(),
        editorControls(state));
}

Ui::Child editorPreview(State const &state) {
    return Ui::image(state.image.unwrap()) |
           Ui::foregroundFilter(state.filter) |
           Ui::spacing(8) |
           Ui::fit();
}

Ui::Child editorToolbar(State const &) {
    return Ui::toolbar(
        Ui::grow(NONE),

        Ui::button(
            Model::bind<ToggleEditor>(),
            Ui::ButtonStyle::subtle(),
            Mdi::Icon::CANCEL,
            "Cancel"),

        Ui::button(
            Model::bind<SaveImage>(),
            Ui::ButtonStyle::primary(),
            Mdi::Icon::FLOPPY,
            "Save Changes"));
}

Ui::Child editorFilterTile(Ui::OnPress onPress, Ui::ButtonStyle style, Mdi::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()) |
           Ui::spacing(8) |
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
        return Mdi::MICROWAVE;
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
            return Ui::slider<decltype(f.amount)>(
                Ui::SliderStyle::regular(),
                f.amount,
                T::RANGE,
                [](auto &n, auto v) {
                    Model::dispatch<SetFilter>(n, T{v});
                });
        }});
}

Ui::Child editorFilters(State const &s) {
    Ui::Children tiles;
    Gfx::Filter::foreach([&]<typename T>(Meta::Type<T>) {
        tiles.pushBack(
            editorFilterTile(
                [](auto &n) {
                    Model::dispatch<SetFilter>(n, T{});
                },
                s.filter.is<T>() ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle(),
                editorFilterIcon<T>(),
                Fmt::toTitleCase(T::NAME).unwrap()));
    });

    return Ui::hflow(8, tiles);
}

Ui::Child editorControls(State const &state) {
    return vflow(
        4,
        editorFilterControls(state.filter),
        editorFilters(state) |
            Ui::spacing({8, 0, 8, 8}) |
            Ui::hscroll());
}

} // namespace ImageViewer

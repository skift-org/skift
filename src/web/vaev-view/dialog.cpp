#include <karm-kira/print-dialog.h>
#include <vaev-driver/render.h>

#include "dialog.h"

namespace Vaev::View {

Style::Media _constructMedia(Print::Settings const &settings) {
    return {
        .type = MediaType::SCREEN,
        .width = Px{settings.paper.width},
        .height = Px{settings.paper.height},
        .aspectRatio = settings.paper.width / (f64)settings.paper.height,
        .orientation = Orientation::LANDSCAPE,

        .resolution = Resolution{settings.scale, Resolution::X},
        .scan = Scan::PROGRESSIVE,
        .grid = false,
        .update = Update::FAST,

        .overflowBlock = OverflowBlock::SCROLL,
        .overflowInline = OverflowInline::SCROLL,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = ColorGamut::SRGB,
        .pointer = Pointer::FINE,
        .hover = Hover::HOVER,
        .anyPointer = Pointer::FINE,
        .anyHover = Hover::HOVER,

        .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
        .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
        .prefersContrast = Contrast::NO_PREFERENCE,
        .forcedColors = Colors::NONE,
        .prefersColorScheme = ColorScheme::DARK,
        .prefersReducedData = ReducedData::NO_PREFERENCE,
    };
}

Ui::Child printDialog(Strong<Markup::Document> dom) {
    return Kr::printDialog([dom](Print::Settings const &settings) {
        return Driver::print(*dom, _constructMedia(settings));
    });
}

} // namespace Vaev::View

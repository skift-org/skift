#include "text.h"

namespace Karm::Ui {

Strong<Media::Fontface> defaultFontface() {
    static Strong<Media::Fontface> f = []() {
        return Media::loadFontface("res/fonts/inter/Inter-Medium.ttf").unwrap();
    }();
    return f;
}

} // namespace Karm::Ui

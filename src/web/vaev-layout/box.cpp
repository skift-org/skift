#include <karm-base/clamp.h>
#include <karm-text/loader.h>

#include "box.h"

namespace Vaev::Layout {

// MARK: Box ------------------------------------------------------------------

Box::Box(Strong<Style::Computed> style, Strong<Text::Fontface> font)
    : style{std::move(style)}, fontFace{font} {}

Box::Box(Strong<Style::Computed> style, Strong<Text::Fontface> font, Content content)
    : style{std::move(style)}, fontFace{font}, content{std::move(content)} {}

Slice<Box> Box::children() const {
    if (auto children = content.is<Vec<Box>>())
        return *children;
    return {};
}

MutSlice<Box> Box::children() {
    if (auto children = content.is<Vec<Box>>()) {
        return *children;
    }
    return {};
}

void Box::add(Box &&box) {
    if (content.is<None>())
        content = Vec<Box>{};

    if (auto children = content.is<Vec<Box>>()) {
        children->pushBack(std::move(box));
    }
}

void Box::repr(Io::Emit &e) const {
    if (children()) {
        e("(box {} {} {} {}", attrs, style->display, style->position, layout.borderBox());
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(box {} {} {} {})", attrs, style->display, style->position, layout.borderBox());
    }
}

} // namespace Vaev::Layout

module;

#include <karm-gfx/colors.h>
#include <karm-gfx/prose.h>
#include <karm-logger/logger.h>

export module Vaev.Engine:layout.backgroundCanvas;

import Karm.Image;
import Karm.Gc;
import Karm.Font;
import :style;
import :dom;
import :layout.values;

namespace Vaev::Layout {

// NOTE This handle only Gfx::Colors but there's a lot more to handle eg Images, Gradients, etc.
Gfx::Color _colorToGfx(Color color) {
    if (auto isRes = color.is<Gfx::Color>()) {
        return *isRes;
    } else {
        logWarn("color was not a Gfx color");
        return Gfx::WHITE;
    }
}

void _patchBackgrounds(MutSlice<Layout::Box>& children) {
    for (auto& child : children) {
        if (child.origin->qualifiedName == Html::BODY_TAG) {
            child.style->backgrounds.cow().color = Gfx::ALPHA;
        }
    }
}

export Gfx::Color fixupBackgrounds(Style::Computer& c, Gc::Ref<Dom::Document> doc, Layout::Tree& tree) {
    auto el = doc->documentElement();
    if (!el) {
        return Gfx::WHITE;
    }

    auto style = c.computeFor(Style::SpecifiedValues::initial(), *el);
    if (style->backgrounds->color != Gfx::ALPHA) {
        tree.root.style->backgrounds.cow().color = Gfx::ALPHA;
        return _colorToGfx(style->backgrounds->color);
    }

    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto isRes = child->is<Dom::Element>()) {
            if (isRes->qualifiedName != Html::BODY_TAG) {
                continue;
            }

            auto childStyle = c.computeFor(*style, *isRes);
            if (childStyle->backgrounds->color != Gfx::ALPHA) {
                auto children = tree.root.children();
                _patchBackgrounds(children);
                return _colorToGfx(childStyle->backgrounds->color);
            } else {
                return Gfx::WHITE;
            }
        }
    }

    return Gfx::WHITE;
}

} // namespace Vaev::Layout

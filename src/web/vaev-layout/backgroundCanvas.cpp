module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:backgroundCanvas;

import :values;

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
        if (child.origin->tagName == Html::BODY) {
            child.style->backgrounds.cow().color = Gfx::ALPHA;
        }
    }
}

export Gfx::Color fixupBackgrounds(Style::Computer& c, Gc::Ref<Dom::Document> doc, Layout::Tree& tree) {
    auto el = doc->documentElement();
    if (!el) {
        return Gfx::WHITE;
    }

    auto style = c.computeFor(Style::ComputedStyle::initial(), *el);
    if (style->backgrounds->color != Gfx::ALPHA) {
        tree.root.style->backgrounds.cow().color = Gfx::ALPHA;
        return _colorToGfx(style->backgrounds->color);
    }

    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto isRes = child->is<Dom::Element>()) {
            if (isRes->tagName != Html::BODY) {
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

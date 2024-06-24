#pragma once

#include <vaev-base/length.h>
#include <vaev-dom/document.h>
#include <vaev-layout/frag.h>
#include <vaev-paint/base.h>

namespace Vaev::View {

struct RenderResult {
    Strong<Layout::Frag> layout;
    Strong<Paint::Node> paint;
};

RenderResult render(Dom::Document const &dom, Vec2Px viewport);

} // namespace Vaev::View

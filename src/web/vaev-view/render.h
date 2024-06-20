#pragma once

#include <vaev-base/length.h>
#include <vaev-dom/document.h>
#include <vaev-layout/base.h>
#include <vaev-paint/base.h>

namespace Vaev::View {

Cons<Strong<Layout::Frag>, Strong<Paint::Node>> render(Dom::Document const &dom, Vec2Px viewport);

} // namespace Vaev::View

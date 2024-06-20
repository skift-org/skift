#pragma once

#include <vaev-base/length.h>
#include <vaev-dom/document.h>
#include <vaev-paint/base.h>

namespace Vaev::View {

Strong<Paint::Node> render(Dom::Document const &dom, RectPx viewport);

} // namespace Vaev::View

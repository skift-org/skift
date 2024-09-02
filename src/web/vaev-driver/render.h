#pragma once

#include <vaev-base/length.h>
#include <vaev-layout/frag.h>
#include <vaev-markup/dom.h>
#include <vaev-paint/base.h>
#include <vaev-style/media.h>

namespace Vaev::Driver {

struct RenderResult {
    Strong<Layout::Frag> layout;
    Strong<Paint::Node> paint;
};

RenderResult render(Markup::Document const &dom, Style::Media const &media, Vec2Px viewport);

RenderResult render(Markup::Document &dom, Style::Media const &media, Print::PaperStock paper);

} // namespace Vaev::Driver

#pragma once

#include <karm-scene/base.h>
#include <vaev-base/length.h>
#include <vaev-layout/box.h>
#include <vaev-markup/dom.h>
#include <vaev-style/media.h>

namespace Vaev::Driver {

struct RenderResult {
    Style::StyleBook style;
    Strong<Layout::Box> layout;
    Strong<Scene::Node> scene;
};

RenderResult render(Markup::Document const &dom, Style::Media const &media, Vec2Px viewport);

RenderResult render(Markup::Document &dom, Style::Media const &media, Print::PaperStock paper);

} // namespace Vaev::Driver

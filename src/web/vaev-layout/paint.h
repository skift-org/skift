#pragma once

#include <karm-scene/stack.h>

#include "frag.h"

namespace Vaev::Layout {

void wireframe(Frag &frag, Gfx::Canvas &g);

void paint(Frag &frag, Scene::Stack &stack);

} // namespace Vaev::Layout

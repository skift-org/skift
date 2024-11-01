#pragma once

#include <karm-scene/stack.h>

#include "box.h"

namespace Vaev::Layout {

void wireframe(Box &box, Gfx::Canvas &g);

void paint(Box &box, Scene::Stack &stack);

} // namespace Vaev::Layout

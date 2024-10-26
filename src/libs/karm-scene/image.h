#pragma once

#include "base.h"

namespace Karm::Scene {

struct Image : public Node {
    void repr(Io::Emit &e) const override {
        e("(image z:{})", zIndex);
    }
};

} // namespace Karm::Scene

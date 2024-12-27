#pragma once

#include "../buffer.h"

namespace Karm::Gfx {

struct CpuSurface {
    virtual ~CpuSurface() = default;

    virtual Math::Recti bound() {
        return mutPixels().bound();
    }

    virtual MutPixels mutPixels() = 0;
};

} // namespace Karm::Gfx

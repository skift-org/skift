#pragma once

#include <karm-base/rc.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>
#include <karm-gfx/surface.h>
#include <karm-meta/utils.h>

#include "events.h"

namespace Karm::Ui {

struct App : public Meta::Static {
    bool _alive = true;

    virtual ~App() = default;

    virtual Gfx::Surface surface() = 0;
    virtual void flip(Slice<Math::Recti> regions) = 0;
    virtual void pump() = 0;
    virtual void wait(size_t ms) = 0;

    void dispatch(Event &event);

    void run() {
        while (_alive) {
            pump();
            wait(16);
        }
    }

    void exit() {
        _alive = false;
    }
};

Result<Strong<App>> makeApp();

} // namespace Karm::Ui

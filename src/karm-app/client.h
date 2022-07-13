#pragma once

#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-meta/utils.h>

namespace Karm::App {

struct Client : public Meta::Static {
    virtual ~Client() = default;

    virtual void paint(Gfx::Context &gfx) = 0;

    virtual void handle(Events::Event &event) = 0;
};

} // namespace Karm::App

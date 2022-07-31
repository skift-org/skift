#pragma once

#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-meta/utils.h>

namespace Karm::App {

struct Host;

struct Client : public Meta::Static {
    Host *_host;

    virtual ~Client() = default;

    Host &host() const;

    virtual void event(Events::Event &) {}

    virtual void layout(Math::Recti) {}

    virtual void paint(Gfx::Context &) {}

    virtual void mount(Host &host);

    virtual void unmount();

    void shouldRepaint(Opt<Math::Recti> = NONE);

    void shouldLayout();

    void shouldAnimate();
};

} // namespace Karm::App

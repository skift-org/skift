#pragma once

#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-meta/utils.h>

namespace Karm::App {

struct Host;

struct Client : public Meta::Static {
    Host *_host;

    virtual ~Client() = default;

    /* --- Callbacks -------------------------------------------------------- */

    virtual void onMount() {}

    virtual void onUnmount() {}

    virtual void onEvent(Events::Event &) {}

    virtual void onLayout(Math::Recti) {}

    virtual void onPaint(Gfx::Context &) {}

    /* --- Mounting / Unmounting -------------------------------------------- */

    Host &host();

    void mount(Host &host);

    void unmount();

    void shouldRepaint(Opt<Math::Recti> = NONE);

    void shouldLayout();

    void shouldAnimate();
};

} // namespace Karm::App

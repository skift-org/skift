#include "client.h"

#include "host.h"
#include "karm-base/try.h"

namespace Karm::App {

Host &Client::host() {
    if (!_host) {
        panic("not mounted");
    }
    return *_host;
}

void Client::mount(Host &host) {
    _host = &host;
    onMount();
}

void Client::unmount() {
    onUnmount();
    _host = nullptr;
}

void Client::shouldRepaint(Opt<Math::Recti> rect) {
    _host->shouldRepaint(tryOr(rect, _host->bound()));
}

void Client::shouldLayout() {
    _host->shouldLayout();
}

void Client::shouldAnimate() {
    _host->shouldAnimate();
}

} // namespace Karm::App

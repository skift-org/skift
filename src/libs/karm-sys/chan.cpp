#include <embed-sys/sys.h>
#include <karm-sys/chan.h>

namespace Karm::Sys {

static In _in{Embed::createIn().take()};
In &in() {
    return _in;
}

static Out _out{Embed::createOut().take()};
Out &out() {
    return _out;
}

static Err _err{Embed::createErr().take()};
Err &err() {
    return _err;
}

} // namespace Karm::Sys

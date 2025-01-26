#include "chan.h"

#include "_embed.h"

namespace Karm::Sys {

static In _in{_Embed::createIn().take()};

In& in() {
    return _in;
}

static Out _out{_Embed::createOut().take()};

Out& out() {
    return _out;
}

static Err _err{_Embed::createErr().take()};

Err& err() {
    return _err;
}

} // namespace Karm::Sys

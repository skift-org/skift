#pragma once

import Karm.Ref;

#include "async.h"

namespace Karm::Sys {

struct Object {
    Ref::Url url;
    Opt<Ref::Uti> type = NONE;

    Object(Ref::Url url) : url(url) {}

    Object(Ref::Url url, Ref::Uti type) : url(url), type(type) {}
};

struct Intent {
    Ref::Uti action;
    Vec<Object> objects;
    Opt<Ref::Url> handler = NONE;
    Opt<Ref::Url> callback = NONE;
};

Res<> launch(Intent intent);

Async::Task<> launchAsync(Intent intent);

} // namespace Karm::Sys

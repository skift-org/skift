#pragma once

#include <karm-mime/uti.h>

#include "async.h"

namespace Karm::Sys {

struct Object {
    Mime::Url url;
    Opt<Mime::Uti> type = NONE;

    Object(Mime::Url url) : url(url) {}

    Object(Mime::Url url, Mime::Uti type) : url(url), type(type) {}
};

struct Intent {
    Mime::Uti action;
    Vec<Object> objects;
    Opt<Mime::Url> handler = NONE;
    Opt<Mime::Url> callback = NONE;
};

Res<> launch(Intent intent);

Async::Task<> launchAsync(Intent intent);

} // namespace Karm::Sys

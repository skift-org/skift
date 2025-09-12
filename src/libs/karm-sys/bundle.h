#pragma once

import Karm.Ref;

namespace Karm::Sys {

struct Bundle {
    String id;

    static Res<Bundle> current();

    static Res<Vec<Bundle>> installed();

    Ref::Url url() const {
        Ref::Url url;
        url.scheme = "bundle"_sym;
        url.host = Symbol::from(id);
        return url;
    }
};

} // namespace Karm::Sys

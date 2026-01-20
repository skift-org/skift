export module Strata.Protos:ifs;

import Karm.Core;
import Karm.Sys;

using namespace Karm;

namespace Strata::IFs {

export using Fid = usize;

export struct Open {
    using Response = Fid;

    Vec<String> path;
};

export struct Close {
    using Response = None;

    Fid fid;
};

export struct Read {
    struct Response {
        Vec<u8> buf;
    };

    Fid fid;
    usize off;
    usize len;
};

export struct Write {
    using Response = usize;

    Fid fid;

    usize off;
    Vec<u8> buf;
};

export struct ReadDir {
    using Response = Vec<Sys::DirEntry>;

    Fid fid;
};

export struct Stat {
    using Response = Sys::Stat;

    Fid fid;
};

} // namespace Strata::IFs
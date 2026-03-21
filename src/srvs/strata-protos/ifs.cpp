export module Strata.Protos:ifs;

import Karm.Core;
import Karm.Sys;
import Karm.Sys.Base;

using namespace Karm;

namespace Strata::IFs {

export using Fid = usize;

export struct Open {
    using Response = Fid;
    enum struct Option {
        OPEN,
        CREATE
    };
    using enum Option;

    Vec<String> path;
    Flags<Option> options;
};

export struct Close {
    using Response = None;
    Fid fid;
};

export struct Read {
    struct Response {
        Vec<u8> buf;
        usize len;
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

export struct Mmap {
    using Response = Rc<Sys::Fd>;
    Fid fid;
};

} // namespace Strata::IFs

export module Strata.Protos:ifs;

import Karm.Core;
import Karm.Sys;
import Karm.Sys.Base;

using namespace Karm;

namespace Strata::IFs {

export struct Read {
    struct Response {
        Vec<u8> buf;
    };

    usize off;
    usize len;
};

export struct Write {
    using Response = usize;

    usize off;
    Vec<u8> buf;
};

export struct ReadDir {
    using Response = Vec<Sys::DirEntry>;
    u8 _unused;
};

export struct Stat {
    using Response = Sys::Stat;
    u8 _unused;
};

export struct Mmap {
    using Response = Rc<Sys::Fd>;
    u8 _unused;
};

} // namespace Strata::IFs

export module Karm.Core:aio.pipe;

import :base.rc;
import :aio.traits;

namespace Karm::Aio {

export struct Pipe {
    bool _closed;

    struct Reader : Aio::Reader {
        Rc<Pipe> _pipe;
    };

    struct Writer : Aio::Writer {
        Rc<Pipe> _pipe;
    };
};

export Pair<Rc<Pipe::Writer>, Rc<Pipe::Reader>> pipe();

} // namespace Karm::Aio

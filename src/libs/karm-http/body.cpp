module;

#include <karm-async/task.h>
#include <karm-io/traits.h>
#include <karm-sys/file.h>

export module Karm.Http:body;

import Karm.Aio;

namespace Karm::Http {

export struct Body : public Aio::Reader {
    static Rc<Body> from(Sys::File);

    static Rc<Body> from(Buf<Byte>);

    static Rc<Body> empty() {
        struct EmptyBody : public Body {
            Async::Task<usize> readAsync(MutBytes) override {
                co_return Ok(0);
            }
        };

        return makeRc<EmptyBody>();
    }
};

} // namespace Karm::Http

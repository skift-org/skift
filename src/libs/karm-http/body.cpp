module;

#include <karm-async/task.h>
#include <karm-io/traits.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>

export module Karm.Http:body;

import Karm.Aio;

namespace Karm::Http {

export struct Body : public Aio::Reader {
    static Rc<Body> from(Sys::FileReader file) {
        struct FileBody : public Body {
            Sys::FileReader _file;

            FileBody(Sys::FileReader file)
                : _file(std::move(file)) {}

            Async::Task<usize> readAsync(MutBytes buf) override {
                co_return _file.read(buf);
            }
        };

        return makeRc<FileBody>(std::move(file));
    }

    static Rc<Body> from(Buf<Byte> buf) {
        struct BufBody : public Body {
            Buf<Byte> _buf;
            Io::BufReader _reader{_buf};

            BufBody(Buf<Byte> buf)
                : _buf(std::move(buf)) {}

            Async::Task<usize> readAsync(MutBytes buf) override {
                co_return _reader.read(buf);
            }
        };

        return makeRc<BufBody>(std::move(buf));
    }

    static Rc<Body> from(String str) {
        return from(str.takeBytes());
    }

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

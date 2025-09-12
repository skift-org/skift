module;

#include <karm-sys/dir.h>
#include <karm-sys/file.h>

export module Karm.Http:body;

import Karm.Core;

namespace Karm::Http {

export struct Body : Aio::Reader {
    static Rc<Body> from(Sys::FileReader file) {
        struct FileBody : Body {
            Sys::FileReader _file;

            FileBody(Sys::FileReader file)
                : _file(std::move(file)) {}

            Async::Task<usize> readAsync(MutBytes buf) override {
                co_return _file.read(buf);
            }
        };

        return makeRc<FileBody>(std::move(file));
    }

    static Rc<Body> from(Buf<u8> buf) {
        struct BufBody : Body {
            Buf<u8> _buf;
            Io::BufReader _reader{_buf};

            BufBody(Buf<u8> buf)
                : _buf(std::move(buf)) {}

            Async::Task<usize> readAsync(MutBytes buf) override {
                co_return _reader.read(buf);
            }
        };

        return makeRc<BufBody>(std::move(buf));
    }

    static Rc<Body> from(Rc<Ref::Blob> blob) {
        struct BlobBody : Body {
            Rc<Ref::Blob> _blob;
            Io::BufReader _reader{_blob->data};

            BlobBody(Rc<Ref::Blob> blob)
                : _blob(std::move(blob)) {}

            Async::Task<usize> readAsync(MutBytes buf) override {
                co_return _reader.read(buf);
            }
        };

        return makeRc<BlobBody>(blob);
    }

    static Rc<Body> from(Str str) {
        return from(bytes(str));
    }

    static Rc<Body> empty() {
        struct EmptyBody : Body {
            Async::Task<usize> readAsync(MutBytes) override {
                co_return Ok(0);
            }
        };

        return makeRc<EmptyBody>();
    }
};

} // namespace Karm::Http

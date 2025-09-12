export module Karm.Kv:blob;

import Karm.Core;

namespace Karm::Kv {

struct _Blob {
    usize len;
    u8 buf[0];
};

export struct Blob;

export struct MutBlob {
    using Inner = u8;

    Rc<_Blob> _buf;
    urange _slice;

    static MutBlob alloc(usize len) {
        using BlobCell = Cell<NoLock, _Blob>;
        auto* buf = new u8[sizeof(BlobCell) + len];
        auto* cell = new (reinterpret_cast<BlobCell*>(buf)) BlobCell(len);
        return {
            {MOVE, cell},
            {0, len},
        };
    }

    static MutBlob from(Bytes bytes) {
        auto blob = MutBlob::alloc(bytes.len());
        copy(bytes, blob.mutBytes());
        return blob;
    }

    usize len() const {
        return _slice.size;
    }

    Bytes bytes() const {
        auto bytes = Bytes{_buf->buf, _buf->len};
        return sub(bytes, _slice);
    }

    MutBytes mutBytes() {
        auto bytes = MutBytes{_buf->buf, _buf->len};
        return mutSub(bytes, _slice);
    }

    MutBlob mutSlice(urange slice) {
        return {
            _buf,
            _slice.slice(slice),
        };
    }

    Blob slice(urange slice) const;

    bool operator==(MutBlob const& other) const {
        return bytes() == other.bytes();
    }

    auto operator<=>(MutBlob const& other) const {
        return bytes() <=> other.bytes();
    }
};

export struct Blob {
    using Inner = u8;

    Rc<_Blob> _buf;
    urange _slice;

    static Blob from(Bytes bytes) {
        return MutBlob::from(bytes);
    }

    Blob(MutBlob const& blob)
        : _buf(blob._buf),
          _slice(blob._slice) {}

    Blob(Rc<_Blob> buf, urange slice)
        : _buf(buf),
          _slice(slice) {}

    usize len() const {
        return _slice.size;
    }

    Bytes bytes() const {
        return sub(Bytes{_buf->buf, _buf->len}, _slice);
    }

    Blob slice(urange slice) const {
        return {
            _buf,
            _slice.slice(slice),
        };
    }

    bool operator==(Blob const& other) const {
        return bytes() == other.bytes();
    }

    auto operator<=>(Blob const& other) const {
        return bytes() <=> other.bytes();
    }
};

Blob MutBlob::slice(urange slice) const {
    return {
        _buf,
        _slice.slice(slice),
    };
}

} // namespace Karm::Kv

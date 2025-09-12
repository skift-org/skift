module;

#include <karm-core/macros.h>

export module Karm.Core:io.impls;

import :base.buf;
import :base.res;
import :io.traits;
import :io.types;

namespace Karm::Io {

export struct Sink : Writer {
    Res<usize> write(Bytes bytes) override {
        return Ok(sizeOf(bytes));
    }
};

export struct Zero : Reader {
    Res<usize> read(MutBytes bytes) override {
        return Ok(zeroFill(bytes));
    }
};

export struct Repeat : Reader {
    u8 _byte{};

    Repeat(u8 byte) : _byte(byte) {}

    Res<usize> read(MutBytes bytes) override {
        return Ok(fill(bytes, _byte));
    }
};

export struct Empty : Reader {
    Res<usize> read(MutBytes) override {
        return Ok(0uz);
    }
};

export struct Count : Writer, Seeker {
    Writer& _reader;
    usize _pos{};

    Count(Writer& reader)
        : _reader(reader) {}

    Res<usize> write(Bytes bytes) override {
        usize written = try$(_reader.write(bytes));
        _pos += written;
        return Ok(written);
    }

    Res<usize> seek(Seek seek) override {
        if (seek != Whence::CURRENT and seek.offset != 0)
            return Error::invalidData("can't seek count reader");
        return Ok(_pos);
    }
};

export template <Readable Readable>
struct Limit : Reader {
    Readable _reader{};
    usize _limit{};
    usize _read{};

    Limit(Readable&& reader, usize limit)
        : _reader(std::forward<Readable>(reader)),
          _limit(limit) {}

    Res<usize> read(MutBytes bytes) override {
        usize size = clamp(sizeOf(bytes), 0uz, _limit - _read);
        usize read = try$(_reader.read(bytes.buf(), size));
        _read += read;
        return Ok(read);
    }
};

export template <SeekableWritable Writable>
struct WriterSlice : Writer, Seeker {
    Writable _writer{};
    usize _start{};
    usize _end{};

    WriterSlice(Writable writer, usize start, usize end)
        : _writer(writer), _start(start), _end(end) {}

    Res<usize> seek(Seek seek) override {
        usize pos = try$(tell(_writer));
        usize s = size(*this);
        pos = seek.apply(pos, s);
        pos = clamp(pos, _start, _end);
        return _writer.seek(Seek::fromBegin(pos));
    }

    Res<usize> write(Bytes bytes) override {
        usize pos = try$(tell(_writer));

        if (pos < _start) {
            try$(_writer.seek(Seek::fromBegin(_start)));
        }

        if (pos > _end) {
            return Ok(0uz);
        }

        usize size = clamp(sizeOf(bytes), 0uz, _end - pos);
        return _writer.write(sub(bytes, 0, size));
    }
};

export template <SeekableWritable Writable>
Res<Slice<Writable>> makeSlice(Writable&& writer, usize size) {
    auto start = try$(writer.tell());
    auto end = start + size;

    return Slice{std::forward<Writable>(writer), start, end};
}

export struct BufReader :
    Reader,
    Seeker {

    Bytes _buf{};
    usize _pos{};

    BufReader(Bytes buf) : _buf(buf), _pos(0) {}

    Res<usize> read(MutBytes bytes) override {
        Bytes slice = sub(_buf, _pos, _pos + sizeOf(bytes));
        usize read = copy(slice, bytes);
        _pos += read;
        return Ok(read);
    }

    Res<usize> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return Ok(_pos);
    }

    Bytes bytes() const {
        return next(_buf, _pos);
    }

    usize rem() const {
        return sizeOf(_buf) - _pos;
    }
};

export struct BufWriter :
    Writer,
    Seeker {

    MutBytes _buf;
    usize _pos = 0;

    BufWriter(MutBytes buf) : _buf(buf) {}

    Res<usize> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return Ok(_pos);
    }

    Res<usize> write(Bytes bytes) override {
        MutBytes slice = mutNext(_buf, _pos);
        usize written = copy(bytes, slice);
        _pos += written;
        return Ok(written);
    }
};

export struct BufferWriter : Writer, Flusher {
    Buf<u8> _buf{};

    BufferWriter(usize cap = 16) : _buf(cap) {}

    Res<usize> write(Bytes bytes) override {
        _buf.insert(COPY, _buf.len(), bytes.buf(), bytes.len());
        return Ok(bytes.len());
    }

    Bytes bytes() const {
        return _buf;
    }

    Res<> flush() override {
        _buf.trunc(0);
        return Ok();
    }

    Buf<u8> take() {
        return std::move(_buf);
    }

    void clear() {
        _buf.trunc(0);
    }
};

export struct BitReader {
    Reader& _reader;
    u8 _bits{};
    u8 _len{};

    BitReader(Reader& reader)
        : _reader(reader) {
    }

    Res<u8> readBit() {
        if (_len == 0) {
            if (try$(_reader.read(MutBytes{&_bits, 1})) == 0)
                return Error::unexpectedEof();
            _len = 8;
        }

        u8 bit = _bits & 1;
        _bits >>= 1;
        _len -= 1;

        return Ok(bit);
    }

    template <Meta::Unsigned T>
    Res<T> readBits(usize n) {
        T bits = 0;
        for (usize i = 0; i < n; i++) {
            bits |= try$(readBit()) << i;
        }
        return Ok(bits);
    }

    void align() {
        _bits = 0;
        _len = 0;
    }

    Res<u8> readByte() {
        align();
        return readBits<u8>(8);
    }

    template <Meta::Unsigned T>
    Res<T> readBytes(usize n) {
        align();
        T bytes = {};
        for (usize i = 0; i < n; i++)
            bytes |= try$(readByte()) << (8 * i);
        return Ok(bytes);
    }
};

} // namespace Karm::Io

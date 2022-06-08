#pragma once

#include <embed/sys.h>
#include <karm-base/bytes.h>
#include <karm-io/traits.h>

#include "types.h"

namespace Karm::Sys {

struct Mmap :
    Meta::NoCopy {
    using enum MmapFlags;

    size_t _paddr;
    void const *_buf{};
    size_t _len{};

    Mmap(void const *buf, size_t len)
        : _paddr(0), _buf(buf), _len(len) {}

    Mmap(size_t paddr, void const *buf, size_t len)
        : _paddr(paddr), _buf(buf), _len(len) {}

    Mmap(Mmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    Mmap &operator=(Mmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        return *this;
    }

    ~Mmap() {
        if (_buf)
            Embed::memUnmap(_buf, _len).unwrap();
    }

    USizeRange vrange() const {
        return {(size_t)_buf, _len};
    }

    USizeRange prange() const {
        return {_paddr, _len};
    }

    Bytes bytes() const { return {_buf, _len}; }

    template <typename T>
    T const *as() const { return static_cast<T const *>(_buf); }

    template <typename T>
    Slice<T> slice() const { return {(T *)_buf, _len / sizeof(T)}; }

    template <typename T>
    Cursor<T> cursor() const { return {(T *)_buf, _len / sizeof(T)}; }

    void const *buf() const { return _buf; }

    size_t len() const { return _len; }

    void leak() { _buf = nullptr; }
};

struct MutMmap :
    public Io::Flusher,
    Meta::NoCopy {
    using enum MmapFlags;

    size_t _paddr;
    void *_buf;
    size_t _len;

    MutMmap(void *buf, size_t len)
        : _paddr(0), _buf(buf), _len(len) {}

    MutMmap(size_t paddr, void *buf, size_t len)
        : _paddr(paddr), _buf(buf), _len(len) {}

    Result<size_t> flush() override {
        return Embed::memFlush(_buf, _len);
    }

    MutMmap(MutMmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    MutMmap &operator=(MutMmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        return *this;
    }

    ~MutMmap() {
        if (_buf)
            Embed::memUnmap(_buf, _len).unwrap();
    }

    USizeRange vrange() const {
        return {(size_t)_buf, _len};
    }

    USizeRange prange() const {
        return {_paddr, _len};
    }

    Bytes bytes() const { return {_buf, _len}; }

    MutBytes bytes() { return {_buf, _len}; }

    template <typename T>
    T *as() { return static_cast<T *>(_buf); }

    template <typename T>
    MutSlice<T> slice() { return MutSlice<T>{(T *)_buf, _len}; }

    template <typename T>
    Cursor<T> cursor() { return Cursor<T>{(T *)_buf, _len}; }

    void *buf() { return _buf; }
    void const *buf() const { return _buf; }
    size_t len() const { return _len; }

    void leak() { _buf = nullptr; }
};

struct _Mmap {
    using enum MmapFlags;

    MmapOptions _options{};
    OptStrong<Fd> _fd;

    _Mmap &read() {
        _options.flags |= READ;
        return *this;
    }

    _Mmap &write() {
        _options.flags |= WRITE;
        return *this;
    }

    _Mmap &exec() {
        _options.flags |= EXEC;
        return *this;
    }

    _Mmap &stack() {
        _options.flags |= STACK;
        return *this;
    }

    _Mmap &paddr(size_t paddr) {
        _options.paddr = paddr;
        return *this;
    }

    _Mmap &vaddr(size_t paddr) {
        _options.vaddr = paddr;
        return *this;
    }

    _Mmap &offset(size_t offset) {
        _options.offset = offset;
        return *this;
    }

    _Mmap &size(size_t size) {
        _options.size = size;
        return *this;
    }

    Result<Mmap> map() {
        _options.flags |= READ;
        USizeRange range = try$(Embed::memMap(_options));
        return Mmap{(void const *)range.start, range.len()};
    }

    Result<Mmap> map(Strong<Fd> fd) {
        _options.flags |= READ;
        USizeRange range = try$(Embed::memMap(_options, fd));
        return Mmap{(void const *)range.start, range.len()};
    }

    Result<Mmap> map(AsFd auto &what) {
        return map(what.asFd());
    }

    Result<MutMmap> mapMut() {
        _options.flags |= WRITE;
        USizeRange range = try$(Embed::memMap(_options));
        return MutMmap{(void *)range.start, range.len()};
    }

    Result<MutMmap> mapMut(Strong<Fd> fd) {
        _options.flags |= WRITE;
        USizeRange range = try$(Embed::memMap(_options, fd));
        return MutMmap{(void *)range.start, range.len()};
    }

    Result<MutMmap> mapMut(AsFd auto &what) {
        return mapMut(what.asFd());
    }
};

static inline _Mmap mmap() {
    return {};
}

} // namespace Karm::Sys

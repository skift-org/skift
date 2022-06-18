#pragma once

#include <embed/sys.h>
#include <karm-base/bytes.h>
#include <karm-io/traits.h>

#include "types.h"

namespace Karm::Sys {

struct Mmap :
    Meta::NoCopy {
    using enum MmapFlags;

    size_t _paddr{};
    void const *_buf{};
    size_t _size{};

    /*
        Mmap(void const *buf, size_t size)
            : _paddr(-1), _buf(buf), _size(size) {}
    */
    Mmap(size_t paddr, void const *buf, size_t size)
        : _paddr(paddr), _buf(buf), _size(size) {}

    Mmap(Mmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
    }

    Mmap &operator=(Mmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
        return *this;
    }

    ~Mmap() {
        if (_buf) {
            Embed::memUnmap(_buf, _size).unwrap("mem-unmap failled");
        }
    }

    size_t vaddr() const {
        return (size_t)_buf;
    }

    size_t paddr() const {
        return _paddr;
    }

    USizeRange vrange() const {
        return {vaddr(), vaddr() + _size};
    }

    USizeRange prange() const {
        return {_paddr, _paddr + _size};
    }

    Bytes bytes() const {
        return {_buf, _size};
    }

    template <typename T>
    T const *as() const {
        return static_cast<T const *>(_buf);
    }

    template <typename T>
    Slice<T> slice() const {
        return {(T *)_buf, _size / sizeof(T)};
    }

    template <typename T>
    Cursor<T> cursor() const {
        return {(T *)_buf, _size / sizeof(T)};
    }

    void const *buf() const {
        return _buf;
    }

    void const *begin() const {
        return _buf;
    }

    void const *end() const {
        return (void *)((size_t)_buf + _size);
    }

    size_t size() const {
        return _size;
    }
};

struct MutMmap :
    public Io::Flusher,
    Meta::NoCopy {
    using enum MmapFlags;

    size_t _paddr{};
    void *_buf{};
    size_t _size{};

    /*
        MutMmap(void *buf, size_t size)
            : _paddr(-1), _buf(buf), _size(size) {}
    */
    MutMmap(size_t paddr, void *buf, size_t size)
        : _paddr(paddr), _buf(buf), _size(size) {}

    Result<size_t> flush() override {
        return Embed::memFlush(_buf, _size);
    }

    MutMmap(MutMmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
    }

    MutMmap &operator=(MutMmap &&other) {
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
        return *this;
    }

    ~MutMmap() {
        if (_buf) {
            Embed::memUnmap(_buf, _size).unwrap("memUnmap failled");
        }
    }

    size_t vaddr() const {
        return (size_t)_buf;
    }

    size_t paddr() const {
        return _paddr;
    }

    USizeRange vrange() const {
        return {(size_t)_buf, (size_t)_buf + _size};
    }

    USizeRange prange() const {
        return {_paddr, _paddr + _size};
    }

    Bytes bytes() const {
        return {_buf, _size};
    }

    MutBytes bytes() {
        return {_buf, _size};
    }

    template <typename T>
    T *as() {
        return static_cast<T *>(_buf);
    }

    template <typename T>
    MutSlice<T> slice() {
        return MutSlice<T>{(T *)_buf, _size / sizeof(T)};
    }

    template <typename T>
    Cursor<T> cursor() {
        return Cursor<T>{(T *)_buf, _size / sizeof(T)};
    }

    void *buf() {
        return _buf;
    }

    void const *buf() const {
        return _buf;
    }

    void *begin() {
        return _buf;
    }

    void const *begin() const {
        return _buf;
    }

    void *end() {
        return (void *)((size_t)_buf + _size);
    }
    void const *end() const {
        return (void *)((size_t)_buf + _size);
    }

    size_t size() const {
        return _size;
    }
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
        MmapResult range = try$(Embed::memMap(_options));
        return Mmap{range.paddr, (void const *)range.vaddr, range.size};
    }

    Result<Mmap> map(Strong<Fd> fd) {
        _options.flags |= READ;
        MmapResult range = try$(Embed::memMap(_options, fd));
        return Mmap{range.paddr, (void const *)range.vaddr, range.size};
    }

    Result<Mmap> map(AsFd auto &what) {
        return map(what.asFd());
    }

    Result<MutMmap> mapMut() {
        _options.flags |= WRITE;
        MmapResult range = try$(Embed::memMap(_options));
        return MutMmap{range.paddr, (void *)range.vaddr, range.size};
    }

    Result<MutMmap> mapMut(Strong<Fd> fd) {
        _options.flags |= WRITE;
        MmapResult result = try$(Embed::memMap(_options, fd));
        return MutMmap{result.paddr, (void *)result.vaddr, result.size};
    }

    Result<MutMmap> mapMut(AsFd auto &what) {
        return mapMut(what.asFd());
    }
};

static inline _Mmap mmap() {
    return {};
}

} // namespace Karm::Sys

#pragma once

import Karm.Core;

#include <karm-sys/_embed.h>

#include "types.h"

namespace Karm::Sys {

struct Mmap :
    Meta::NoCopy {
    using enum MmapOption;

    usize _paddr{};
    void const* _buf{};
    usize _size{};
    bool _owned{true};

    static Res<Mmap> createUnowned(void const* buf, usize size) {
        return Ok(Mmap{0, buf, size, false});
    }

    Mmap(usize paddr, void const* buf, usize size, bool owned = true)
        : _paddr(paddr), _buf(buf), _size(size), _owned(owned) {}

    Mmap(Mmap&& other) {
        std::swap(_paddr, other._paddr);
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
    }

    Mmap& operator=(Mmap&& other) {
        std::swap(_paddr, other._paddr);
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
        return *this;
    }

    ~Mmap() {
        unmap().unwrap("unmap failed");
    }

    Res<> unmap() {
        if (_buf and _owned) {
            try$(_Embed::memUnmap(std::exchange(_buf, nullptr), _size));
            _paddr = 0;
            _size = 0;
        }
        return Ok();
    }

    usize vaddr() const { return reinterpret_cast<usize>(_buf); }

    usize paddr() const { return _paddr; }

    urange vrange() const { return {vaddr(), _size}; }

    urange prange() const { return {_paddr, _size}; }

    template <typename T>
    T const* as() const {
        return static_cast<T const*>(_buf);
    }

    template <typename T>
    Cursor<T> cursor() const {
        return Cursor<T>{(T*)_buf, _size / sizeof(T)};
    }

    Bytes bytes() const {
        return {static_cast<u8 const*>(_buf), _size};
    }

    void leak() {
        _owned = false;
    }
};

struct MutMmap :
    Io::Flusher,
    Meta::NoCopy {
    using enum MmapOption;

    usize _paddr{};
    void* _buf{};
    usize _size{};
    bool _owned{true};

    static Res<MutMmap> createUnowned(void* buf, usize size) {
        return Ok(MutMmap{0, buf, size, false});
    }

    MutMmap(usize paddr, void* buf, usize size, bool owned = true)
        : _paddr(paddr), _buf(buf), _size(size), _owned(owned) {
    }

    Res<> flush() override {
        try$(_Embed::memFlush(_buf, _size));
        return Ok();
    }

    MutMmap(MutMmap&& other) {
        std::swap(_paddr, other._paddr);
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
    }

    MutMmap& operator=(MutMmap&& other) {
        std::swap(_paddr, other._paddr);
        std::swap(_buf, other._buf);
        std::swap(_size, other._size);
        return *this;
    }

    ~MutMmap() {
        unmap().unwrap("unmap failed");
    }

    Res<> unmap() {
        if (_buf and _owned) {
            try$(_Embed::memUnmap(std::exchange(_buf, nullptr), _size));
            _paddr = 0;
            _buf = nullptr;
            _size = 0;
        }
        return Ok();
    }

    usize vaddr() const {
        return (usize)_buf;
    }

    usize paddr() const {
        return _paddr;
    }

    urange vrange() const {
        return {(usize)_buf, _size};
    }

    urange prange() const {
        return {_paddr, _size};
    }

    template <typename T>
    T const* as() const {
        return static_cast<T const*>(_buf);
    }

    template <typename T>
    T* as() {
        return static_cast<T*>(_buf);
    }

    template <typename T>
    Cursor<T> cursor() const {
        return Cursor<T>{(T*)_buf, _size / sizeof(T)};
    }

    template <typename T>
    MutCursor<T> mutCursor() {
        return Cursor<T>{(T*)_buf, _size / sizeof(T)};
    }

    Bytes bytes() const {
        return {static_cast<u8 const*>(_buf), _size};
    }

    MutBytes mutBytes() {
        return {static_cast<u8*>(_buf), _size};
    }

    void leak() {
        _owned = false;
    }
};

struct _Mmap {
    using enum MmapOption;

    MmapProps _props{};
    Opt<Rc<Fd>> _fd;

    _Mmap& read() {
        _props.options |= READ;
        return *this;
    }

    _Mmap& write() {
        _props.options |= WRITE;
        return *this;
    }

    _Mmap& exec() {
        _props.options |= EXEC;
        return *this;
    }

    _Mmap& stack() {
        _props.options |= STACK;
        return *this;
    }

    _Mmap& paddr(usize paddr) {
        _props.paddr = paddr;
        return *this;
    }

    _Mmap& vaddr(usize paddr) {
        _props.vaddr = paddr;
        return *this;
    }

    _Mmap& offset(usize offset) {
        _props.offset = offset;
        return *this;
    }

    _Mmap& size(usize size) {
        _props.size = size;
        return *this;
    }

    Res<Mmap> map() {
        _props.options |= READ;
        MmapResult range = try$(_Embed::memMap(_props));
        return Ok(Mmap{range.paddr, (void const*)range.vaddr, range.size});
    }

    Res<Mmap> map(Rc<Fd> fd) {
        _props.options |= READ;
        MmapResult range = try$(_Embed::memMap(_props, fd));
        return Ok(Mmap{range.paddr, (void const*)range.vaddr, range.size});
    }

    Res<Mmap> map(AsFd auto& what) {
        return map(what.fd());
    }

    Res<MutMmap> mapMut() {
        _props.options |= WRITE;
        MmapResult range = try$(_Embed::memMap(_props));
        return Ok(MutMmap{range.paddr, (void*)range.vaddr, range.size});
    }

    Res<MutMmap> mapMut(Rc<Fd> fd) {
        _props.options |= WRITE;
        MmapResult result = try$(_Embed::memMap(_props, fd));
        return Ok(MutMmap{result.paddr, (void*)result.vaddr, result.size});
    }

    Res<MutMmap> mapMut(AsFd auto& what) {
        return mapMut(what.fd());
    }
};

inline _Mmap mmap() {
    return {};
}

} // namespace Karm::Sys

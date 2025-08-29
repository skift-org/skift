#pragma once

import Karm.Core;

#include <hal/io.h>
#include <hjert-api/api.h>

namespace Strata::Device {

struct PortIo : public Hal::Io {
    Hj::Io _io;

    PortIo(Hj::Io io)
        : _io(std::move(io)) {}

    static Res<Rc<Hal::Io>> open(Hal::PortRange range) {
        auto io = try$(Hj::Io::create(Hj::ROOT, range.start, range.size));
        return Ok(makeRc<PortIo>(std::move(io)));
    }

    Res<usize> in(usize offset, usize size) {
        return _io.in(Hj::bytes2IoLen(size), offset);
    }

    Res<> out(usize offset, usize size, usize value) {
        return _io.out(Hj::bytes2IoLen(size), offset, value);
    }
};

struct DmaIo : public Hal::Io {
    Hj::Mapped _mapped;

    DmaIo(Hj::Mapped mapped)
        : _mapped(std::move(mapped)) {}

    static Res<Rc<Hal::Io>> open(Hal::DmaRange range) {
        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, range.start, range.size, Hj::VmoFlags::DMA));
        auto mapped = try$(Hj::map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        return Ok(makeRc<DmaIo>(std::move(mapped)));
    }

    Res<usize> in(usize offset, usize size) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        if (size == 1) {
            return Ok(*((u8 volatile*)(addr)));
        } else if (size == 2) {
            return Ok(*((u16 volatile*)(addr)));
        } else if (size == 4) {
            return Ok(*((u32 volatile*)(addr)));
        } else if (size == 8) {
            return Ok(*((u64 volatile*)(addr)));
        } else {
            return Error::invalidInput("unsupported size");
        }
    }

    Res<> out(usize offset, usize size, usize value) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        if (size == 1) {
            *((u8 volatile*)(addr)) = value;
        } else if (size == 2) {
            *((u16 volatile*)(addr)) = value;
        } else if (size == 4) {
            *((u32 volatile*)(addr)) = value;
        } else if (size == 8) {
            *((u64 volatile*)(addr)) = value;
        } else {
            return Error::invalidInput("unsupported size");
        }

        return Ok();
    }

    Res<> read(usize offset, MutBytes buf) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            buf[i] = *((u8 volatile*)(addr + i));
        }

        return Ok();
    }

    Res<> write(usize offset, Bytes buf) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            *((u8 volatile*)(addr + i)) = buf[i];
        }

        return Ok();
    }
};

} // namespace Strata::Device

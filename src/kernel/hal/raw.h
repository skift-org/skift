#pragma once

#include <karm-logger/logger.h>

#include "io.h"

namespace Hal {

struct RawPortIo : Io {
    PortRange _range;

    RawPortIo(PortRange range) : _range(range) {
    }

    Res<usize> in(usize offset, [[maybe_unused]] usize size) override {
        u16 addr = _range.start + offset;

        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

#ifdef __ck_arch_x86_64__

        if (size == 1) {
            u8 res = 0;
            asm volatile("inb %1, %0"
                         : "=a"(res)
                         : "d"(addr));
            return Ok(res);
        } else if (size == 2) {
            u16 res = 0;
            asm volatile("inw %1, %0"
                         : "=a"(res)
                         : "d"(addr));
            return Ok(res);
        } else if (size == 4) {
            u32 res = 0;
            asm volatile("inl %1, %0"
                         : "=a"(res)
                         : "d"(addr));
            return Ok(res);
        } else if (size == 8) {
            // x86_64 doesn't support 64bit I/O operation so we emulate it
            u64 res = try$(in32(addr));
            res |= ((u64)try$(in32(addr + 4)) << 32);
            return Ok(res);
        } else {
            return Error::invalidInput("unsupported size");
        }
#else
        return Error::invalidInput("not implemented");
#endif
    }

    Res<> out(usize offset, [[maybe_unused]] usize size, [[maybe_unused]] usize value) override {
        u16 addr = _range.start + offset;

        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

#ifdef __ck_arch_x86_64__
        if (size == 1) {
            u8 val = value;
            asm volatile("outb %0, %1"
                         :
                         : "a"(val), "d"(addr));
        } else if (size == 2) {
            u16 val = value;
            asm volatile("outw %0, %1"
                         :
                         : "a"(val), "d"(addr));
        } else if (size == 4) {
            u32 val = value;
            asm volatile("outl %0, %1"
                         :
                         : "a"(val), "d"(addr));
        } else if (size == 8) {
            // x86_64 doesn't support 64bit I/O operation so we emulate it
            try$(out32(addr, value));
            try$(out32(addr + 4, value >> 32));
        } else {
            return Error::invalidInput("unsupported size");
        }

        return Ok();
#else
        return Error::invalidInput("not implemented");
#endif
    }
};

struct RawDmaIo : Io {
    DmaRange _range;

    RawDmaIo(DmaRange range) : _range(range) {
    }

    Res<usize> in(usize offset, usize size) override {
        usize addr = _range.start + offset;
        if (not _range.contains(addr))
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
        usize addr = _range.start + offset;
        if (not _range.contains(addr))
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
        usize addr = _range.start + offset;
        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            buf[i] = *((u8 volatile*)(addr + i));
        }

        return Ok();
    }

    Res<> write(usize offset, Bytes buf) override {
        usize addr = _range.start + offset;
        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            *((u8 volatile*)(addr + i)) = buf[i];
        }

        return Ok();
    }
};

} // namespace Hal

#pragma once

#include <hal/io.h>
#include <hjert-api/api.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-events/events.h>

namespace Dev {

struct PortIo : public Hal::Io {
    Hj::Io _io;

    PortIo(Hj::Io io)
        : _io(std::move(io)) {}

    static inline Res<Strong<Hal::Io>> open(Hal::PortRange range) {
        auto io = try$(Hj::Io::create(Hj::ROOT, range.start, range.size));
        return Ok(makeStrong<PortIo>(std::move(io)));
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

    static inline Res<Strong<Hal::Io>> open(Hal::DmaRange range) {
        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, range.start, range.size, Hj::VmoFlags::DMA));
        auto mapped = try$(Hj::map(vmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        return Ok(makeStrong<DmaIo>(std::move(mapped)));
    }

    Res<usize> in(usize offset, usize size) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        if (size == 1) {
            return Ok(*((u8 volatile *)(addr)));
        } else if (size == 2) {
            return Ok(*((u16 volatile *)(addr)));
        } else if (size == 4) {
            return Ok(*((u32 volatile *)(addr)));
        } else if (size == 8) {
            return Ok(*((u64 volatile *)(addr)));
        } else {
            return Error::invalidInput("unsupported size");
        }
    }

    Res<> out(usize offset, usize size, usize value) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        if (size == 1) {
            *((u8 volatile *)(addr)) = value;
        } else if (size == 2) {
            *((u16 volatile *)(addr)) = value;
        } else if (size == 4) {
            *((u32 volatile *)(addr)) = value;
        } else if (size == 8) {
            *((u64 volatile *)(addr)) = value;
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
            buf[i] = *((u8 volatile *)(addr + i));
        }

        return Ok();
    }

    Res<> write(usize offset, Bytes buf) override {
        usize addr = _mapped.range().start + offset;
        if (not _mapped.range().contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            *((u8 volatile *)(addr + i)) = buf[i];
        }

        return Ok();
    }
};

struct IrqEvent : public Events::BaseEvent<IrqEvent> {
    u8 irq;

    IrqEvent(u8 irq)
        : irq(irq) {}
};

struct Node : public Meta::Static {
    Node *_parent = nullptr;
    Vec<Strong<Node>> _children = {};

    virtual ~Node() {
        for (auto &child : _children) {
            child->_parent = nullptr;
        }
    }

    virtual Res<> init() {
        for (auto &child : _children) {
            try$(child->init());
        }
        return Ok();
    }

    virtual Res<> event(Events::Event &e) {
        if (e.accepted) {
            return Ok();
        }

        for (auto &child : _children) {
            try$(child->event(e));

            if (e.accepted) {
                return Ok();
            }
        }

        return Ok();
    }

    virtual Res<> bubble(Events::Event &e) {
        if (_parent and not e.accepted) {
            try$(_parent->bubble(e));
        }

        return Ok();
    }

    Res<> attach(Strong<Node> child) {
        child->_parent = this;
        _children.pushBack(child);
        try$(child->init());
        return Ok();
    }

    void detach(Strong<Node> child) {
        child->_parent = nullptr;
        _children.removeAll(child);
    }

    Ordr cmp(Node const &other) const {
        if (this == &other) {
            return Ordr::EQUAL;
        } else {
            return Ordr::LESS;
        }
    }
};

} // namespace Dev

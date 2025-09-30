module;

#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-math/rect.h>

export module Strata.Protos:base;

import Karm.Core;
import Karm.Gfx;

using namespace Karm;

namespace Strata::Protos {

export struct Ack{
    Res<> serialize(Serde::Serializer& ser) const {
        return ser.serializeUnit({Serde::Type::UNIT});
    }

    static Res<Ack> deserialize(Serde::Deserializer& de) {
        try$(de.deserializeUnit({Serde::Type::NIL}));
        return Ok(Ack{});
    }
};

export Ack ACK{};

export struct Surface {
    Hj::Vmo _vmo;
    Hj::Mapped _mapped;

    Math::Vec2i _size;
    usize _stride;
    Gfx::Fmt _fmt;

    static Res<Rc<Surface>> from(Hj::Vmo vmo, Math::Vec2i size) {
        auto mapped = try$(map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        return Ok(makeRc<Surface>(std::move(vmo), std::move(mapped), size, size.x * Gfx::RGBA8888.bpp(), Gfx::RGBA8888));
    }

    static Res<Rc<Surface>> create(Math::Vec2i size) {
        auto len = size.x * size.y * Gfx::RGBA8888.bpp();
        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, 0, alignUp(len, 4096)));
        return from(std::move(vmo), size);
    }

    Gfx::Pixels pixels() const {
        return {
            _mapped.bytes().buf(),
            _size,
            _stride,
            _fmt,
        };
    }

    Gfx::MutPixels mutPixels() {
        return {
            _mapped.mutBytes().buf(),
            _size,
            _stride,
            _fmt,
        };
    }

    Res<> serialize(Serde::Serializer& ser) const {
        auto scope = try$(ser.beginScope({.kind = Serde::Type::OBJECT}));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _vmo));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _size));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _stride));
        return scope.end();
    }

    static Res<Surface> deserialize(Serde::Deserializer& de) {
        auto scope = try$(de.beginScope({.kind = Serde::Type::OBJECT}));
        auto [_, vmo] = try$(scope.deserializeUnit<Hj::Vmo>({.kind = Serde::Type::OBJECT_ITEM}));
        auto [_, size] = try$(scope.deserializeUnit<Math::Vec2i>({.kind = Serde::Type::OBJECT_ITEM}));
        auto [_, stride] = try$(scope.deserializeUnit<usize>({.kind = Serde::Type::OBJECT_ITEM}));
        try$(scope.end());

        auto mapped = try$(map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        return Ok(Surface{std::move(vmo), std::move(mapped), size, stride, Gfx::RGBA8888});
    }
};

} // namespace Strata::Protos

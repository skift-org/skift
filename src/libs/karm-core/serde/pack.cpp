module;

#include <karm-core/macros.h>

export module Karm.Core:serde.pack;

import :io;
import :serde.serde;

// Pack is a simple schema-less serialization format use for IPCs.
namespace Karm::Serde {

export struct PackSerializer : Serializer {
    Io::BEmit& _emit;

    PackSerializer(Io::BEmit& emit) : _emit(emit) {}

    Res<> beginUnit(Type type) override {
        switch (type.kind) {
        case Type::NIL:
            _emit.writeFrom<u8>(0);
            break;

        case Type::SOME:
            _emit.writeFrom<u8>(1);
            break;

        case Type::UNION:
            break;

        case Type::UNION_ITEM:
            _emit.writeFrom<u8>(type.index.unwrap());
            break;

        case Type::ENUM:
            break;
        case Type::ENUM_ITEM:
            break;

        case Type::ARRAY:
            break;

        case Type::VEC:
            try$(serializeUnsigned(type.len.unwrap(), SizeHint::AUTO));
            break;

        case Type::MAP:
            try$(serializeUnsigned(type.len.unwrap(), SizeHint::AUTO));
            break;
        case Type::MAP_ITEM:
            try$(serializeString(type.tag.unwrap().str()));
            break;

        case Type::OBJECT:
            break;

        case Type::OBJECT_ITEM:
            break;
        }

        return Ok();
    }

    Res<> endUnit() override {
        return Ok();
    }

    Res<> serializeBool(bool v) override {
        _emit.writeFrom<u8>(v);
        return Ok();
    }

    Res<> serializeInteger(i64 v, SizeHint) override {
        _emit.writeFrom<i64>(v);
        return Ok();
    }

    Res<> serializeUnsigned(u64 v, SizeHint) override {
        _emit.writeFrom<u64>(v);
        return Ok();
    }

    Res<> serializeFloat(f64 v, SizeHint) override {
        _emit.writeFrom<f64>(v);
        return Ok();
    }

    Res<> serializeBytes(Bytes v) override {
        _emit.writeFrom<u64>(v.len());
        _emit.writeBytes(v);
        return Ok();
    }

    Res<> serializeString(Str v) override {
        _emit.writeFrom<u64>(v.len());
        _emit.writeStr(v);
        return Ok();
    }
};

export struct PackDeserializer : Deserializer {
    Io::BScan& _scan;

    PackDeserializer(Io::BScan& scan) : _scan(scan) {}

    Res<Type> beginUnit(Type type) override {
        Type res = type;
        switch (type.kind) {
        case Type::NIL:
            if (_scan.peek<u8>() != 0)
                return Error::invalidData("expected nil");
            _scan.next<u8>();
            break;

        case Type::SOME:
            if (_scan.peek<u8>() != 1)
                return Error::invalidData("expected some");
            _scan.next<u8>();
            break;

        case Type::UNION:
            break;

        case Type::UNION_ITEM:
            if (_scan.peek<u8>() != type.index)
                return Error::invalidData("union index mismatch");
            res.index = _scan.next<u8>();
            break;

        case Type::ENUM:
            break;

        case Type::ENUM_ITEM:
            break;

        case Type::ARRAY:
            break;

        case Type::VEC:
            res.len = try$(deserializeUnsigned(SizeHint::AUTO));
            break;

        case Type::MAP:
            res.len = try$(deserializeUnsigned(SizeHint::AUTO));
            break;

        case Type::MAP_ITEM:
            res.tag = Symbol::from(try$(deserializeString()));
            break;

        case Type::OBJECT:
            break;

        case Type::OBJECT_ITEM:
            break;
        }

        return Ok(res);
    }

    Res<bool> endedUnit() override {
        return Ok(false);
    }

    Res<> endUnit() override {
        return Ok();
    }

    Res<bool> deserializeBool() override {
        return Ok(_scan.next<u8>());
    }

    Res<i64> deserializeInteger(SizeHint) override {
        return Ok(_scan.next<i64>());
    }

    Res<u64> deserializeUnsigned(SizeHint) override {
        return Ok(_scan.next<u64>());
    }

    Res<f64> deserializeFloat(SizeHint) override {
        return Ok(_scan.next<f64>());
    }

    Res<Vec<u8>> deserializeBytes() override {
        usize len = _scan.next<u64>();
        return Ok(_scan.nextBytes(len));
    }

    Res<String> deserializeString() override {
        usize len = _scan.next<u64>();
        return Ok(_scan.nextStr(len));
    }
};

} // namespace Karm::Serde

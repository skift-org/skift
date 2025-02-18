#pragma once

#include "value.h"

namespace Vaev::Script {

// https://tc39.es/ecma262/#property-key
struct PropertyKey {
    using _Store = Union<String, Symbol, u64>;

    _Store store;

    static PropertyKey from(String str) {
        return {str};
    }

    static PropertyKey from(Symbol sym) {
        return {sym};
    }

    static PropertyKey from(u64 num) {
        return {num};
    }

    bool operator==(PropertyKey const& other) const {
        return store == other.store;
    }
};

// https://tc39.es/ecma262/#sec-property-attributes
struct PropertyDescriptor {
    Value value = undefined;
    Opt<Boolean> writable = NONE;
    Opt<Gc::Ptr<Object>> get = NONE;
    Opt<Gc::Ptr<Object>> set = NONE;
    Opt<Boolean> enumerable = NONE;
    Opt<Boolean> configurable = NONE;

    bool empty() const {
        return value == undefined and
               writable == NONE and
               get == NONE and
               set == NONE and
               enumerable == NONE and
               configurable == NONE;
    }

    // https://tc39.es/ecma262/#sec-isaccessordescriptor
    Boolean isAccessorDescriptor() const {
        // 1. If Desc is undefined, return false.

        // 2. If Desc has a [[Get]] or [[Set]] field, return true.
        if (get != NONE or set != NONE)
            return true;

        // 3. Return false.
        return false;
    }

    // https://tc39.es/ecma262/#sec-isdatadescriptor
    Boolean isDataDescriptor() const {
        // 1. If Desc is undefined, return false.

        // 2. If Desc has a [[Value]] field, return true.
        if (value != undefined)
            return true;

        // 3. If Desc has a [[Writable]] field, return true.
        if (writable)
            return true;

        // 4. Return false.
        return false;
    }

    // https://tc39.es/ecma262/#sec-isgenericdescriptor
    bool isGenericDescriptor() const {
        // 1. If Desc is undefined, return false.

        // 2. If IsAccessorDescriptor(Desc) is true, return false.
        // 3. If IsDataDescriptor(Desc) is true, return false.
        if (isAccessorDescriptor() or isDataDescriptor())
            return false;

        // 4. Return true.
        return true;
    }
};

struct PropertyStorage {
    struct Accessor {
        Gc::Ptr<Object> get;
        Gc::Ptr<Object> set;
    };

    struct Attributes {
        bool writable;
        bool enumerable;
        bool configurable;
    };

    struct Property {
        Union<Value, Accessor> value;
        Attributes attributes;
    };

    Map<PropertyKey, Property> _props;

    void set(PropertyKey key, Property prop) {
        _props.put(key, prop);
    }

    Opt<Property> get(PropertyKey key) {
        return _props.tryGet(key);
    }
};

} // namespace Vaev::Script

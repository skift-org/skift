#pragma once

#include <karm-base/func.h>
#include <karm-base/vec.h>

#include "agent.h"
#include "completion.h"
#include "properties.h"
#include "value.h"

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-object-type
struct Object;

// MARK: Ordinary Object Internal Methods and Internal Slots -------------------
// https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots

Except<Boolean> ordinaryDefineOwnProperty(Object& self, PropertyKey key, PropertyDescriptor desc);

Except<Value> ordinaryGet(Object& self, PropertyKey key, Value receiver);

struct InternalMethods {
    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-getprototypeof
    Except<Gc::Ptr<Object>> (*getPrototypeOf)(Object& self) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-setprototypeof-v
    Except<Boolean> (*setPrototypeOf)(Object& self, Gc::Ptr<Object> v) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-isextensible
    Except<Boolean> (*isExtensible)(Object& self) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-preventextensions
    Except<Boolean> (*preventExtensions)(Object& self) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-getownproperty-p
    Except<Opt<PropertyDescriptor>> (*getOwnProperty)(Object& self, PropertyKey key) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-defineownproperty-p-desc
    Except<Boolean> (*defineOwnProperty)(Object& self, PropertyKey key, PropertyDescriptor desc) = ordinaryDefineOwnProperty;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-hasproperty-p
    Except<Boolean> (*hasProperty)(Object& self, PropertyKey key) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-get-p-receiver
    Except<Value> (*get)(Object& self, PropertyKey key, Value receiver) = ordinaryGet;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-set-p-v-receiver
    Except<Boolean> (*set)(Object& self, PropertyKey key, Value v, Value receiver) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-delete-p
    Except<Boolean> (*delete_)(Object& self, PropertyKey key) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-ownpropertykeys
    Except<Vec<PropertyKey>> (*ownPropertyKeys)(Object& self) = nullptr;

    // Additional Essential Internal Methods of Function Objects
    // https://tc39.es/ecma262/#table-additional-essential-internal-methods-of-function-objects

    Except<Value> (*call)(Object& self, Gc::Ref<Object> thisArg, Slice<Value> args) = nullptr;

    Except<Value> (*construct)(Object& self, Slice<Value> args, Gc::Ref<Object> newTarget) = nullptr;
};

// MARK: The Object Type -------------------------------------------------------
// https://tc39.es/ecma262/#sec-object-type

struct _ObjectCreateArgs {
    Gc::Ptr<Object> prototype = nullptr;
};

struct Object {
    Agent& agent;
    InternalMethods internalMethods = {};
    PropertyStorage propertyStorage = {};
    Gc::Ptr<Object> prototype = nullptr;

    static Gc::Ref<Object> create(Agent& agent, _ObjectCreateArgs args = {});

    Except<Gc::Ptr<Object>> getPrototypeOf();

    Except<Boolean> setPrototypeOf(Gc::Ptr<Object> v);

    Except<Boolean> isExtensible();

    Except<Boolean> preventExtensions();

    Except<Opt<PropertyDescriptor>> getOwnProperty(PropertyKey key);

    Except<Boolean> defineOwnProperty(PropertyKey key, PropertyDescriptor desc);

    Except<Boolean> hasProperty(PropertyKey key);

    Except<Value> get(PropertyKey key, Value receiver);

    Except<Boolean> set(PropertyKey key, Value value, Value receiver);

    Except<Boolean> delete_(PropertyKey key);

    Except<Vec<PropertyKey>> ownPropertyKeys();

    Except<Value> call(Gc::Ref<Object> thisArg, Slice<Value> args);

    Except<Value> construct(Slice<Value> args, Gc::Ref<Object> newTarget);

    void repr(Io::Emit& e) const;

    Gc::Ref<Object> ref() {
        return *this;
    }
};

} // namespace Vaev::Script

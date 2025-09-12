module;

#include <karm-core/macros.h>

module Vaev.Script;

import Karm.Core;
import Karm.Gc;

namespace Vaev::Script {

// MARK: The Object Type -------------------------------------------------------
// https://tc39.es/ecma262/#sec-object-type

Gc::Ref<Object> Object::create(Agent& agent, _ObjectCreateArgs args) {
    auto obj = agent.heap.alloc<Object>(agent);
    obj->prototype = args.prototype;
    return obj;
}

Except<Gc::Ptr<Object>> Object::getPrototypeOf() {
    if (not internalMethods.getPrototypeOf)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.getPrototypeOf(*this);
}

Except<Boolean> Object::setPrototypeOf(Gc::Ptr<Object> v) {
    if (not internalMethods.setPrototypeOf)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.setPrototypeOf(*this, v);
}

Except<Boolean> Object::isExtensible() {
    if (not internalMethods.isExtensible)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.isExtensible(*this);
}

Except<Boolean> Object::preventExtensions() {
    if (not internalMethods.preventExtensions)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.preventExtensions(*this);
}

Except<Opt<PropertyDescriptor>> Object::getOwnProperty(PropertyKey key) {
    if (not internalMethods.getOwnProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.getOwnProperty(*this, key);
}

Except<Boolean> Object::defineOwnProperty(PropertyKey key, PropertyDescriptor desc) {
    if (not internalMethods.defineOwnProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.defineOwnProperty(*this, key, desc);
}

Except<Boolean> Object::hasProperty(PropertyKey key) {
    if (not internalMethods.hasProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.hasProperty(*this, key);
}

Except<Value> Object::get(PropertyKey key, Value receiver) {
    if (not internalMethods.get)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.get(*this, key, receiver);
}

Except<Boolean> Object::set(PropertyKey key, Value value, Value receiver) {
    if (not internalMethods.set)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.set(*this, key, value, receiver);
}

Except<Boolean> Object::delete_(PropertyKey key) {
    if (not internalMethods.delete_)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.delete_(*this, key);
}

Except<Vec<PropertyKey>> Object::ownPropertyKeys() {
    if (not internalMethods.ownPropertyKeys)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.ownPropertyKeys(*this);
}

Except<Value> Object::call(Gc::Ref<Object> thisArg, Slice<Value> args) {
    if (not internalMethods.call)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.call(*this, thisArg, args);
}

Except<Value> Object::construct(Slice<Value> args, Gc::Ref<Object> newTarget) {
    if (not internalMethods.construct)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.construct(*this, args, newTarget);
}

void Object::repr(Io::Emit& e) const {
    e("[object Object]");
}

Gc::Ref<Object> Object::ref() {
    return *this;
}

// MARK: Ordinary Object Ordinary Methods --------------------------------------
// https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots

// https://tc39.es/ecma262/#sec-validateandapplypropertydescriptor
static Except<Boolean> _validateAndApplyPropertyDescriptor(Gc::Ptr<Object> self, PropertyKey key, bool extensible, PropertyDescriptor desc, Opt<PropertyDescriptor> current) {
    // 1. Assert: P is a property key.
    // 2. If current is undefined, then
    if (current == NONE) {
        //    a. If extensible is false, return false.
        if (not extensible)
            return Ok(false);

        //    b. If O is undefined, return true.
        if (not self)
            return Ok(true);

        auto& object = *self;

        //    c. If IsAccessorDescriptor(Desc) is true, then
        if (desc.isAccessorDescriptor()) {
            // i. Create an own accessor property named P of object O whose
            //    [[Get]], [[Set]], [[Enumerable]], and [[Configurable]]
            //    attributes are set to the value of the corresponding field
            //    in Desc if Desc has that field, or to the attribute's
            //    default value otherwise.
            object.propertyStorage.set(
                key,
                {
                    .value = PropertyStorage::Accessor{
                        .get = desc.get.unwrapOr(nullptr),
                        .set = desc.set.unwrapOr(nullptr),
                    },
                    .attributes = {
                        .writable = false,
                        .enumerable = desc.enumerable.unwrapOr(false),
                        .configurable = desc.configurable.unwrapOr(false),
                    },
                }
            );

            // d. Else,
        } else {
            // i. Create an own data property named P of object O whose
            //    [[Value]], [[Writable]], [[Enumerable]], and [[Configurable]]
            //    attributes are set to the value of the corresponding field in
            //    Desc if Desc has that field, or to the attribute's default
            //    value otherwise.
            object.propertyStorage.set(
                key,
                {
                    .value = desc.value,
                    .attributes = {
                        .writable = desc.writable.unwrapOr(false),
                        .enumerable = desc.enumerable.unwrapOr(false),
                        .configurable = desc.configurable.unwrapOr(false),
                    },
                }
            );
        }
        //    e. Return true.
        return Ok(true);
    }

    // 3. Assert: current is a fully populated Property Descriptor.

    // 4. If Desc does not have any fields, return true.
    if (desc.empty())
        return Ok(true);

    // 5. If current.[[Configurable]] is false, then
    if (not current->configurable) {
        //    a. If Desc has a [[Configurable]] field and Desc.[[Configurable]] is true, return false.
        if (desc.configurable and desc.configurable == true)
            return Ok(false);

        //    b. If Desc has an [[Enumerable]] field and Desc.[[Enumerable]] is not current.[[Enumerable]], return false.
        if (desc.enumerable and desc.enumerable != current->enumerable)
            return Ok(false);

        //    c. If IsGenericDescriptor(Desc) is false and IsAccessorDescriptor(Desc) is not IsAccessorDescriptor(current), return false.
        if (not desc.isGenericDescriptor() and desc.isAccessorDescriptor() != current->isAccessorDescriptor())
            return Ok(false);

        //    d. If IsAccessorDescriptor(current) is true, then
        if (current->isGenericDescriptor()) {

            //       i. If Desc has a [[Get]] field and SameValue(Desc.[[Get]], current.[[Get]]) is false, return false.
            //       ii. If Desc has a [[Set]] field and SameValue(Desc.[[Set]], current.[[Set]]) is false, return false.

            //    e. Else if current.[[Writable]] is false, then
        } else if (current->writable == false) {
            //       i. If Desc has a [[Writable]] field and Desc.[[Writable]] is true, return false.
            if (desc.writable != NONE and desc.writable == true)
                return Ok(false);

            //       ii. NOTE: SameValue returns true for NaN values which may be distinguishable by other means. Returning here ensures that any existing property of O remains unmodified.
            //       iii. If Desc has a [[Value]] field, return SameValue(Desc.[[Value]], current.[[Value]]).
        }
    }

    // 6. If O is not undefined, then
    if (self) {

        //    a. If IsDataDescriptor(current) is true and IsAccessorDescriptor(Desc) is true, then
        if (current->isDataDescriptor() and desc.isAccessorDescriptor()) {
            //       i. If Desc has a [[Configurable]] field, let configurable be Desc.[[Configurable]]; else let configurable be current.[[Configurable]].
            //       ii. If Desc has a [[Enumerable]] field, let enumerable be Desc.[[Enumerable]]; else let enumerable be current.[[Enumerable]].
            //       iii. Replace the property named P of object O with an accessor property whose [[Configurable]] and [[Enumerable]] attributes are set to configurable and enumerable, respectively, and whose [[Get]] and [[Set]] attributes are set to the value of the corresponding field in Desc if Desc has that field, or to the attribute's default value otherwise.

            //    b. Else if IsAccessorDescriptor(current) is true and IsDataDescriptor(Desc) is true, then
        } else if (current->isAccessorDescriptor() and desc.isDataDescriptor()) {
            //       i. If Desc has a [[Configurable]] field, let configurable be Desc.[[Configurable]]; else let configurable be current.[[Configurable]].
            //       ii. If Desc has a [[Enumerable]] field, let enumerable be Desc.[[Enumerable]]; else let enumerable be current.[[Enumerable]].
            //       iii. Replace the property named P of object O with a data property whose [[Configurable]] and [[Enumerable]] attributes are set to configurable and enumerable, respectively, and whose [[Value]] and [[Writable]] attributes are set to the value of the corresponding field in Desc if Desc has that field, or to the attribute's default value otherwise.

            //    c. Else,
        } else {
            //       i. For each field of Desc, set the corresponding attribute of the property named P of object O to the value of the field.
        }
    }
    // 7. Return true.
    return Ok(true);
}

// https://tc39.es/ecma262/#sec-ordinarydefineownproperty
Except<Boolean> ordinaryDefineOwnProperty(Object& self, PropertyKey key, PropertyDescriptor desc) {
    // 1. Let current be ? O.[[GetOwnProperty]](P).
    auto current = try$(self.getOwnProperty(key));

    // 2. Let extensible be ? IsExtensible(O).
    auto extensible = try$(self.isExtensible());

    // 3. Return ValidateAndApplyPropertyDescriptor(O, P, extensible, Desc, current).
    return _validateAndApplyPropertyDescriptor(self, key, extensible, desc, current);
}

// https://tc39.es/ecma262/#sec-ordinaryget
Except<Value> ordinaryGet(Object& self, PropertyKey key, Value receiver) {
    // 1. Let desc be ? O.[[GetOwnProperty]](P).
    auto maybeDesc = try$(self.getOwnProperty(key));

    // 2. If desc is undefined, then
    if (maybeDesc == undefined) {
        //    a. Let parent be ? O.[[GetPrototypeOf]]().
        auto parent = try$(self.getPrototypeOf());

        //    b. If parent is null, return undefined.
        if (parent == NONE)
            return Ok(undefined);

        //    c. Return ? parent.[[Get]](P, Receiver).
        return parent->get(key, receiver);
    }

    auto& desc = maybeDesc.unwrap();

    // 3. If IsDataDescriptor(desc) is true, return desc.[[Value]].
    if (desc.isDataDescriptor())
        return Ok(desc.value);

    // 4. Assert: IsAccessorDescriptor(desc) is true.
    if (not desc.isAccessorDescriptor())
        panic("expected accessor descriptor");

    // 5. Let getter be desc.[[Get]].
    auto getter = desc.get;

    // 6. If getter is undefined, return undefined.
    if (getter == NONE)
        return Ok(undefined);

    // 7. Return ? Call(getter, Receiver).
    return call(self.agent, *getter, receiver);
}

} // namespace Vaev::Script
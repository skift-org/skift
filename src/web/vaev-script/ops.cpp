#include "ops.h"

#include "object.h"

namespace Vaev::Script {

// MARK: Abstract Operations ---------------------------------------------------
// https://tc39.es/ecma262/#sec-abstract-operations

// MARK: Type Conversion -------------------------------------------------------

// https://tc39.es/ecma262/#sec-ordinarytoprimitive
Except<Value> ordinaryToPrimitive(Object& o, PreferedType hint) {
    Array methodNames = {
        u"toString"_s16,
        u"valueOf"_s16,
    };
    // 1. If hint is string, then
    if (hint == PreferedType::STRING) {
        //    a. Let methodNames be « "toString", "valueOf" ».
        methodNames = {
            u"toString"_s16,
            u"valueOf"_s16,
        };
    }
    // 2. Else,
    //    a. Let methodNames be « "valueOf", "toString" ».

    // 3. For each element name of methodNames, do
    for (auto name : methodNames) {
        //    a. Let method be ? Get(O, name).
        auto method = try$(get(o, PropertyKey::from(name)));

        //    b. If IsCallable(method) is true, then
        if (isCallable(method)) {
            //       i. Let result be ? Call(method, O).
            auto result = try$(call(o.agent, method, o.ref()));

            //       ii. If result is not an Object, return result.
            if (not result.isObject())
                return Ok(result);
        }
    }

    // 4. Throw a TypeError exception.
    return throwException(createException(o.agent, ExceptionType::TYPE_ERROR));
}

// MARK: Testing and Comparison Operations -------------------------------------

// https://tc39.es/ecma262/#sec-iscallable
bool isCallable(Value v) {
    // 1. If argument is not an Object, return false.
    if (not v.isObject())
        return false;

    // 2. If argument has a [[Call]] internal method, return true.
    if (v.asObject()->internalMethods.call)
        return true;

    // 3. Return false.
    return false;
}

// https://tc39.es/ecma262/#sec-sametype
bool sameType(Value x, Value y) {
    // 1. If x is undefined and y is undefined, return true.
    if (x == undefined and y == undefined)
        return true;

    // 2. If x is null and y is null, return true.
    if (x == null and y == null)
        return true;

    // 3. If x is a Boolean and y is a Boolean, return true.
    if (x.isBoolean() and y.isBoolean())
        return true;

    // 4. If x is a Number and y is a Number, return true.
    if (x.isNumber() and y.isNumber())
        return true;

    // 5. If x is a BigInt and y is a BigInt, return true.
    // FIXME: BigInt support.
    // if (x.isBigInt() and y.isBigInt())
    //     return true;

    // 6. If x is a Symbol and y is a Symbol, return true.
    if (x.isSymbol() and y.isSymbol())
        return true;

    // 7. If x is a String and y is a String, return true.
    if (x.isString() and y.isString())
        return true;

    // 8. If x is an Object and y is an Object, return true.
    if (x.isObject() and y.isObject())
        return true;

    // 9. Return false.
    return false;
}

// https://tc39.es/ecma262/#sec-samevalue
bool sameValue(Value x, Value y) {
    // 1. If SameType(x, y) is false, return false.
    if (not sameType(x, y))
        return false;

    // 2. If x is a Number, then
    //    a. Return Number::sameValue(x, y).
    if (x.isNumber())
        return x.asNumber().sameValue(y.asNumber());

    // 3. Return SameValueNonNumber(x, y).
    return sameValueNonNumber(x, y);
}

// https://tc39.es/ecma262/#sec-samevaluenonnumber
bool sameValueNonNumber(Value x, Value y) {
    // 1. Assert: SameType(x, y) is true.
    if (not sameType(x, y))
        panic("expected same type");

    // 2. If x is either null or undefined, return true.
    if (x == null or x == undefined)
        return true;

    // 3. If x is a BigInt, then
    //    a. Return BigInt::equal(x, y).
    // FIXME: BigInt support.

    // 4. If x is a String, then
    //    a. If x and y have the same length and the same code units in the same positions, return true;
    //       otherwise, return false.
    if (x.asString() == y.asString())
        return true;

    // 5. If x is a Boolean, then
    //    a. If x and y are both true or both false, return true; otherwise, return false.
    if (x.isBoolean())
        return x.asBoolean() == y.asBoolean();

    // 6. NOTE: All other ECMAScript language values are compared by identity.
    // 7. If x is y, return true; otherwise, return false.
    // https://tc39.es/ecma262/#sec-identity
    if (x.isSymbol())
        return x.asSymbol() == y.asSymbol();

    if (not x.isObject())
        panic("expected object");

    return x.asObject().checkIdentity(y.asObject());
}

// MARK: Operations on Objects -------------------------------------------------

// https://tc39.es/ecma262/#sec-get-o-p
Except<Value> get(Object& o, PropertyKey p) {
    // 1. Return ? O.[[Get]](P, O).
    return o.get(p, o.ref());
}

// https://tc39.es/ecma262/#sec-call
Except<Value> call(Agent& agent, Value f, Value v, Slice<Value> args) {
    // 1. If argumentsList is not present, set argumentsList to a new empty List.
    // 2. If IsCallable(F) is false, throw a TypeError exception.
    if (not isCallable(f))
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));

    // 3. Return ? F.[[Call]](V, argumentsList)
    return f.asObject()->call(v.asObject(), args);
}

// MARK: Operations on Iterator Objects ----------------------------------------

// MARK: Runtime Semantics -----------------------------------------------------

// https://tc39.es/ecma262/#sec-throw-an-exception

Gc::Ref<Object> createException(Agent& agent, ExceptionType) {
    // FIXME: Implement this properly.
    auto exception = Object::create(agent);
    exception->set(
                 PropertyKey::from(u"name"_s16),
                 String{u"Exception"_s16},
                 exception
    )
        .unwrap();
    return exception;
}

Completion throwException(Value exception) {
    // 1. Return ThrowCompletion(a newly created TypeError object).
    return Completion::throw_(exception);
}

} // namespace Vaev::Script

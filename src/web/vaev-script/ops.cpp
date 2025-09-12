module;

#include <karm-core/macros.h>

export module Vaev.Script:ops;

import Karm.Core;
import Karm.Gc;

import :agent;
import :completion;
import :properties;
import :value;
import :object;

namespace Vaev::Script {

// MARK: Runtime Semantics =====================================================

export enum struct ExceptionType {
    ERROR,
    TYPE_ERROR,
    RANGE_ERROR,
    REFERENCE_ERROR,
    SYNTAX_ERROR,
    URI_ERROR,

    _LEN,
};

export Gc::Ref<Object> createException(Agent& agent, ExceptionType type);

// https://tc39.es/ecma262/#sec-throw-an-exception
export Completion throwException(Value exception);

// MARK: Abstract Operations ===================================================
// https://tc39.es/ecma262/#sec-abstract-operations

// MARK: Type Conversion -------------------------------------------------------

// https://tc39.es/ecma262/#sec-toprimitive
enum struct PreferedType {
    DEFAULT,
    STRING,
    NUMBER,
};
Except<Value> toPrimitive(Value v, PreferedType preferredType = PreferedType::DEFAULT);

// https://tc39.es/ecma262/#sec-ordinarytoprimitive
Except<Value> ordinaryToPrimitive(Object& o, PreferedType hint = PreferedType::DEFAULT);

// MARK: Testing and Comparison Operations -------------------------------------

// https://tc39.es/ecma262/#sec-iscallable
bool isCallable(Value v);

// https://tc39.es/ecma262/#sec-sametype
bool sameType(Value x, Value y);

// https://tc39.es/ecma262/#sec-samevalue
bool sameValue(Value x, Value y);

// https://tc39.es/ecma262/#sec-samevaluenonnumber
bool sameValueNonNumber(Value x, Value y);

// MARK: Operations on Objects -------------------------------------------------

// https://tc39.es/ecma262/#sec-get-o-p
Except<Value> get(Object& o, PropertyKey p);

// https://tc39.es/ecma262/#sec-call
Except<Value> call(Agent& agent, Value f, Value v, Slice<Value> args = {});

// MARK: Operations on Iterator Objects ----------------------------------------

} // namespace Vaev::Script

#include "values.h"

namespace Karm::Json {

Res<> stringify(Io::Emit &emit, Value const &v) {
    return v.visit(
        Visitor{
            [&](None) -> Res<> {
                emit("null");
                return Ok();
            },
            [&](Vec<Value> const &v) -> Res<> {
                emit('[');
                for (usize i = 0; i < v.len(); ++i) {
                    if (i > 0) {
                        emit(',');
                    }
                    try$(stringify(emit, v[i]));
                }
                emit(']');

                return Ok();
            },
            [&](Map<String, Value> const &m) -> Res<> {
                emit('{');
                bool first = true;
                for (auto const &kv : m.iter()) {
                    if (not first) {
                        emit(',');
                    }
                    first = false;

                    emit('"');
                    emit(kv.car);
                    emit("\":");
                    try$(stringify(emit, kv.cdr));
                }
                emit('}');
                return Ok();
            },
            [&](String const &s) -> Res<> {
                emit('"');
                for (auto c : iterRunes(s)) {
                    if (c == '"') {
                        emit("\\\"");
                    } else if (c == '\\') {
                        emit("\\\\");
                    } else if (c == '\b') {
                        emit("\\b");
                    } else if (c == '\f') {
                        emit("\\f");
                    } else if (c == '\n') {
                        emit("\\n");
                    } else if (c == '\r') {
                        emit("\\r");
                    } else if (c == '\t') {
                        emit("\\t");
                    } else if (c < 0x20) {
                        emit("\\u{x}", c);
                    } else {
                        emit(c);
                    }
                }
                emit('"');
                return Ok();
            },
            [&](Integer i) -> Res<> {
                emit("{}", i);
                return Ok();
            },
#ifndef __ck_freestanding__
            [&](Number d) -> Res<> {
                emit("{}", d);
                return Ok();
            },
#endif
            [&](bool b) -> Res<> {
                emit(b ? "true" : "false");
                return Ok();
            },
        }
    );
}

Res<String> stringify(Value const &v) {
    Io::StringWriter sw;
    Io::Emit emit{sw};
    try$(stringify(emit, v));
    return Ok(sw.take());
}

} // namespace Karm::Json

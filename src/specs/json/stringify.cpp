#include "json.h"

namespace Json {

Error stringify(Text::Emit &emit, Value const &v) {
    return v.visit(
        Visitor{
            [&](None) {
                emit("null");

                return OK;
            },
            [&](Vec<Value> const &v) {
                emit('[');
                for (size_t i = 0; i < v.len(); ++i) {
                    if (i > 0) {
                        emit(',');
                    }
                    try$(stringify(emit, v[i]));
                }
                emit(']');

                return OK;
            },
            [&](Map<String, Value> const &m) {
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
                return OK;
            },
            [&](String const &s) {
                emit('"');
                for (auto c : s) {
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
                return OK;
            },
            [&](Number d) {
                emit("{}", d);
                return OK;
            },
            [&](bool b) {
                emit(b ? "true" : "false");
                return OK;
            },
        });
}

Result<String> stringify(Value const &v) {
    Io::StringWriter sw;
    Text::Emit emit{sw};
    try$(stringify(emit, v));
    return sw.take();
}

} // namespace Json

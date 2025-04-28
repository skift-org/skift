#pragma once

#include <karm-base/string.h>
#include <karm-io/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Io {

struct Emit : Io::TextWriter {
    Io::TextWriter& _writer;
    usize _ident = 0;
    Res<> _error = Ok();
    bool _newline = false;

    Emit(Io::TextWriter& writer)
        : _writer(writer) {
    }

    void _tryWrapper(Res<> result) {
        if (not result)
            _error = result.none();
    }

    void indent() {
        _ident++;
    }

    void indentNewline() {
        indent();
        newline();
    }

    void indented(auto inner) {
        indent();
        inner();
        deindent();
    }

    void deindent() {
        if (_ident == 0) [[unlikely]]
            panic("deident() underflow");

        _ident--;
    }

    void newline() {
        if (not _error)
            return;

        _newline = true;
    }

    Res<> _insertNewline() {
        if (not _error)
            return _error.none();
        try$(_writer.writeRune('\n'));
        _newline = false;
        for (usize i = 0; i < _ident; i++)
            try$(_writer.writeStr("    "s));
        return Ok();
    }

    virtual Res<usize> write(Bytes bytes) override {
        auto written = try$(_writer.write(bytes));
        return Ok(written);
    }

    Res<> writeRune(Rune r) override {
        if (r == '\n') {
            newline();
            return Ok();
        }

        if (_newline)
            try$(_insertNewline());

        return _writer.writeRune(r);
    }

    void operator()(Rune r) {
        if (r == '\n') {
            newline();
            return;
        }
        _tryWrapper(writeRune(r));
    }

    void operator()(Str str) {
        _tryWrapper(writeStr(str));
    }

    template <typename... Ts>
    void operator()(Str fmt, Ts&&... ts) {
        _tryWrapper(Io::format(*this, fmt, std::forward<Ts>(ts)...));
    }

    template <typename... Ts>
    void ln(Ts&&... ts) {
        _tryWrapper(Io::format(*this, std::forward<Ts>(ts)...));
        newline();
    }

    Res<> flush() override {
        try$(_error);
        if (_newline)
            try$(_insertNewline());
        return Ok();
    }
};

template <ReprMethod T>
struct Repr<T> {
    static void repr(Io::Emit& emit, T const& val) {
        val.repr(emit);
    }
};

template <Reprable T>
void repr(Io::Emit& emit, T const& val) {
    if constexpr (ReprMethod<T>) {
        val.repr(emit);
    } else {
        Repr<T>::repr(emit, val);
    }
}

template <Reprable T>
struct Formatter<T> {
    Res<> format(Io::TextWriter& writer, T const& val) {
        Io::Emit emit{writer};
        repr(emit, val);
        return emit.flush();
    }
};

} // namespace Karm::Io

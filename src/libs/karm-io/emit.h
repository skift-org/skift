#pragma once

#include <karm-base/string.h>
#include <karm-fmt/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Io {

struct Emit : public Io::TextWriterBase<> {
    Io::TextWriter &_writer;
    usize _ident = 0;
    usize _total = 0;
    Res<> _error = Ok();
    bool _newline = false;

    Emit(Io::TextWriter &writer)
        : _writer(writer) {
    }

    void _tryWrapper(Res<usize> result) {
        if (result) {
            _total += result.unwrap();
        } else {
            _error = result.none();
        }
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
        if (_ident == 0) {
            panic("deident() underflow");
        }
        _ident--;
    }

    void newline() {
        if (not _error)
            return;

        _newline = true;
    }

    void insertNewline() {
        if (not _error)
            return;

        _tryWrapper(_writer.writeRune('\n'));
        for (usize i = 0; i < _ident; i++) {
            _tryWrapper(_writer.writeStr("    "));
        }

        _newline = false;
    }

    virtual Res<usize> write(Bytes bytes) override {
        return _writer.write(bytes);
    }

    Res<usize> writeRune(Rune r) override {
        if (r == '\n') {
            newline();
        } else {
            _tryWrapper(_writer.writeRune(r));
        }
        return Ok(1uz);
    }

    void operator()(Rune r) {
        if (not _error)
            return;

        if (r == '\n') {
            newline();
            return;
        }
        _tryWrapper(_writer.writeRune(r));
    }

    void operator()(Str str) {
        if (not _error)
            return;

        if (_newline) {
            insertNewline();
        }

        _tryWrapper(_writer.writeStr(str));
    }

    template <typename... Ts>
    void operator()(Str format, Ts &&...ts) {
        if (not _error)
            return;

        if (_newline) {
            insertNewline();
        }

        _tryWrapper(Fmt::format(*this, format, std::forward<Ts>(ts)...));
    }

    template <typename... Ts>
    void ln(Ts &&...ts) {
        if (not _error)
            return;

        if (_newline) {
            insertNewline();
        }

        _tryWrapper(Fmt::format(*this, std::forward<Ts>(ts)...));
        newline();
    }

    usize total() {
        return _total;
    }
};

} // namespace Karm::Text

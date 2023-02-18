#pragma once

#include <karm-base/string.h>
#include <karm-fmt/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Text {

struct Emit {
    Io::_TextWriter &_writer;
    size_t _ident = 0;
    size_t _total = 0;
    Res<> _error = Ok();
    bool _newline = false;

    Emit(Io::_TextWriter &writer)
        : _writer(writer) {
    }

    void _tryWrapper(Res<size_t> result) {
        if (result) {
            _total += result.unwrap();
        } else {
            _error = result.none();
        }
    }

    void indent() {
        _ident++;
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
        for (size_t i = 0; i < _ident; i++) {
            _tryWrapper(_writer.writeStr("    "));
        }

        _newline = false;
    }

    void operator()(Rune r) {
        if (not _error)
            return;

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

        _tryWrapper(Fmt::format(_writer, format, std::forward<Ts>(ts)...));
    }

    size_t total() {
        return _total;
    }
};

} // namespace Karm::Text

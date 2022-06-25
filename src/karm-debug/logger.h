#pragma once

#include <karm-base/loc.h>
#include <karm-fmt/fmt.h>

namespace Karm::Debug {

struct Logger {
    enum struct Level {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };

    Str _name;

    constexpr Logger(Str name) : _name(name) {}

    void _log(Level level, Str format, Fmt::_Args &args, Loc loc);

    void debug(Str format, auto &&...va, Loc loc = Loc::current()) {
        Fmt::Args<decltype(va)...> args{std::forward<decltype(va)>(va)...};
        _log(Level::DEBUG, format, args, loc);
    }

    void info(Str format, auto &&...va, Loc loc = Loc::current()) {
        Fmt::Args<decltype(va)...> args{std::forward<decltype(va)>(va)...};
        _log(Level::INFO, format, args, loc);
    }

    void warn(Str format, auto &&...va, Loc loc = Loc::current()) {
        Fmt::Args<decltype(va)...> args{std::forward<decltype(va)>(va)...};
        _log(Level::WARN, format, args, loc);
    }

    void error(Str format, auto &&...va, Loc loc = Loc::current()) {
        Fmt::Args<decltype(va)...> args{std::forward<decltype(va)>(va)...};
        _log(Level::ERROR, format, args, loc);
    }

    [[noreturn]] void panic(Str format, auto &&...va, Loc loc = Loc::current()) {
        Fmt::Args<decltype(va)...> args{std::forward<decltype(va)>(va)...};
        _log(Level::FATAL, format, args, loc);
        panic("_log(FATAL) returned");
    }
};

} // namespace Karm::Debug

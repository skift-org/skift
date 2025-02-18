#pragma once

#include <karm-base/loc.h>
#include <karm-cli/style.h>
#include <karm-io/fmt.h>

#include "_embed.h"

namespace Karm {

struct Level {
    isize value;
    char const* name;
    Cli::Style style;
};

struct Format {
    Str str;
    Loc loc;

    Format(char const* str, Loc loc = Loc::current())
        : str(str), loc(loc) {
    }

    Format(Str str, Loc loc = Loc::current())
        : str(str), loc(loc) {
    }
};

static constexpr Level PRINT = {-2, "print", Cli::BLUE};
static constexpr Level YAP = {-1, "yappin'", Cli::GREEN};
static constexpr Level DEBUG = {0, "debug", Cli::BLUE};
static constexpr Level INFO = {1, "info ", Cli::GREEN};
static constexpr Level WARNING = {2, "warn ", Cli::YELLOW};
static constexpr Level ERROR = {3, "error", Cli::RED};
static constexpr Level FATAL = {4, "fatal", Cli::style(Cli::RED).bold()};

inline void _catch(Res<> res) {
    if (res)
        return;
    debug("failed to write to logger");
    panic(res.none().msg());
}

inline void _log(Level level, Format fmt, Io::_Args& args) {
    Logger::_Embed::loggerLock();

    if (level.value != -2) {
        _catch(Io::format(Logger::_Embed::loggerOut(), "{} ", Cli::styled(level.name, level.style)));
        _catch(Io::format(Logger::_Embed::loggerOut(), "{}{}:{}: ", Cli::reset().fg(Cli::GRAY_DARK), fmt.loc.file, fmt.loc.line));
    }

    _catch(Io::format(Logger::_Embed::loggerOut(), "{}", Cli::reset()));
    _catch(Io::_format(Logger::_Embed::loggerOut(), fmt.str, args));
    _catch(Io::format(Logger::_Embed::loggerOut(), "{}\n", Cli::reset()));
    _catch(Logger::_Embed::loggerOut().flush());

    Logger::_Embed::loggerUnlock();
}

template <typename... Args>
inline void logPrint(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(PRINT, fmt, args);
}

template <typename... Args>
inline void logPrintIf(bool condition, Format fmt, Args&&... va) {
    if (condition)
        logPrint(fmt, std::forward<Args>(va)...);
}

template <typename... Args>
inline void logDebug(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(DEBUG, fmt, args);
}

template <typename... Args>
inline void logDebugIf(bool condition, Format fmt, Args&&... va) {
    if (condition)
        logDebug(fmt, std::forward<Args>(va)...);
}

template <typename... Args>
inline void logInfo(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(INFO, fmt, args);
}

template <typename... Args>
inline void logInfoIf(bool condition, Format fmt, Args&&... va) {
    if (condition)
        logInfo(fmt, std::forward<Args>(va)...);
}

template <typename... Args>
inline void yap(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(YAP, fmt, args);
}

template <typename... Args>
inline void logWarn(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(WARNING, fmt, args);
}

template <typename... Args>
inline void logWarnIf(bool condition, Format fmt, Args&&... va) {
    if (condition)
        logWarn(fmt, std::forward<Args>(va)...);
}

template <typename... Args>
inline void logError(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(ERROR, fmt, args);
}

template <typename... Args>
inline void logErrorIf(bool condition, Format fmt, Args&&... va) {
    if (condition)
        logError(fmt, std::forward<Args>(va)...);
}

template <typename... Args>
[[noreturn]] inline void logFatal(Format fmt, Args&&... va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(FATAL, fmt, args);
    panic("fatal error occured, see logs");
}

} // namespace Karm

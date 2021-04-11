#pragma once

#include <libsystem/Common.h>

namespace Utils
{

class SourceLocation
{
private:
    const char *_file;
    const char *_function;
    uint32_t _line;
    uint32_t _column;

public:
    constexpr const char *file() const noexcept { return _file; }
    constexpr const char *function() const noexcept { return _function; }
    constexpr uint32_t column() const noexcept { return _column; }
    constexpr uint32_t line() const noexcept { return _line; }

    static constexpr SourceLocation
    current(const char *filename = __builtin_FILE(),
            const char *function = __builtin_FUNCTION(),
            uint32_t line = __builtin_LINE(),
            uint32_t column = 0) noexcept
    {
        return SourceLocation{
            filename,
            function,
            line,
            column,
        };
    }

    constexpr SourceLocation(const char *file, const char *function, uint32_t line, uint32_t column)
        : _file{file}, _function{function}, _line{line}, _column{column}
    {
    }

    constexpr SourceLocation()
        : _file{"unknown"}, _function{"unknown"}, _line{0}, _column{0}
    {
    }
};

} // namespace Utils

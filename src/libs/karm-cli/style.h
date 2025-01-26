#pragma once

#include <karm-io/emit.h>

namespace Karm::Cli {

enum Color {
    _COLOR_UNDEF = -1,

    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    GRAY_LIGHT = 7,

    GRAY_DARK = 60,
    RED_LIGHT = 61,
    GREEN_LIGHT = 62,
    YELLOW_LIGHT = 63,
    BLUE_LIGHT = 64,
    MAGENTA_LIGHT = 65,
    CYAN_LIGHT = 66,
    WHITE = 67,
};

static inline Color random(usize seed) {
    if (seed & 1) {
        return (Color)(((seed >> 1) % 7) + 1);
    } else {
        return (Color)((((seed >> 1) % 7) + 1) + 60);
    }
}

// clang-format off

inline constexpr Array LIGHT_COLORS = {
    GRAY_DARK,RED_LIGHT,GREEN_LIGHT,YELLOW_LIGHT,BLUE_LIGHT,MAGENTA_LIGHT,CYAN_LIGHT,WHITE,
};

inline constexpr Array DARK_COLORS = {
    BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,GRAY_LIGHT,
};

inline constexpr Array COLORS = {
    BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,GRAY_LIGHT,
    GRAY_DARK,RED_LIGHT,GREEN_LIGHT,YELLOW_LIGHT,BLUE_LIGHT,MAGENTA_LIGHT,CYAN_LIGHT,WHITE,
};

// clang-format on

struct Style {
    bool _reset{};
    Color _fg{_COLOR_UNDEF};
    Color _bg{_COLOR_UNDEF};
    bool _bold{};
    bool _underline{};
    bool _blink{};
    bool _reverse{};
    bool _invisible{};

    constexpr Style() = default;

    constexpr Style(Color fg)
        : _fg(fg) {
    }

    constexpr Style(Color fg, Color bg)
        : _fg(fg), _bg(bg) {
    }

    constexpr Style fg(Color color) {
        _fg = color;
        return *this;
    }

    constexpr Style bg(Color color) {
        _bg = color;
        return *this;
    }

    constexpr Style bold() {
        _bold = true;
        return *this;
    }

    constexpr Style underline() {
        _underline = true;
        return *this;
    }

    constexpr Style blink() {
        _blink = true;
        return *this;
    }

    constexpr Style reverse() {
        _reverse = true;
        return *this;
    }

    constexpr Style invisible() {
        _invisible = true;
        return *this;
    }

    constexpr Style reset() {
        _reset = true;
        return *this;
    }

    void repr([[maybe_unused]] Io::Emit& e) const {

#ifdef __ck_sys_terminal_ansi__
        if (_reset) {
            e("\x1b[0m"s);
        }

        if (_fg != Karm::Cli::_COLOR_UNDEF) {
            e("\x1b[{}m", _fg + 30);
        }

        if (_bg != Karm::Cli::_COLOR_UNDEF) {
            e("\x1b[{}m", _bg + 40);
        }

        if (_bold) {
            e("\x1b[1m"s);
        }

        if (_underline) {
            e("\x1b[4m"s);
        }

        if (_blink) {
            e("\x1b[5m"s);
        }

        if (_reverse) {
            e("\x1b[7m"s);
        }

        if (_invisible) {
            e("\x1b[8m"s);
        }
#endif
    }
};

inline constexpr Style reset() {
    Style style;
    style._reset = true;
    return style;
};

inline constexpr Style style(auto... args) {
    return Style{args...};
}

template <typename T>
struct Styled {
    T _inner;
    Style _color;
};

inline auto styled(auto inner, Style style) {
    return Styled<decltype(inner)>{inner, style};
}

} // namespace Karm::Cli

template <typename T>
struct Karm::Io::Formatter<Karm::Cli::Styled<T>> {
    Formatter<Karm::Cli::Style> _styleFmt{};
    Formatter<T> _innerFmt{};

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter& writer, Karm::Cli::Styled<T> const& val) {
#ifdef __ck_sys_terminal_ansi__
        return Ok(
            try$(_styleFmt.format(writer, val._color)) +
            try$(_innerFmt.format(writer, val._inner)) +
            try$(writer.writeStr("\x1b[0m"s))
        );
#else
        return _innerFmt.format(writer, val._inner);
#endif
    }
};

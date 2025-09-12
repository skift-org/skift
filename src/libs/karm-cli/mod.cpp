module;

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

export module Karm.Cli;

import Karm.Core;
import Karm.Debug;

namespace Karm::Cli {
// MARK: Tokenizer -------------------------------------------------------------

export struct Token {
    enum struct Kind {
        OPERAND,
        OPTION,
        FLAG,
        EXTRA,

        _LEN,
    };

    using enum Kind;

    Kind kind;
    Rune flag = 0;
    Str value = "";

    Token(Str value)
        : Token(Kind::OPERAND, value) {}

    Token(Kind kind, Str value)
        : kind(kind), value(value) {}

    Token(Kind kind)
        : kind(kind) {}

    Token(Rune flag)
        : kind(Kind::FLAG), flag(flag) {}

    bool operator==(Token const& other) const = default;

    void repr(Io::Emit& e) const {
        if (kind == Kind::FLAG)
            e("(token {} {#c})", kind, flag);
        else
            e("(token {} {#})", kind, value);
    }
};

export void tokenize(Str arg, Vec<Token>& out) {
    if (arg == "--"s) {
        out.pushBack(Token::EXTRA);
    } else if (arg == "-"s) {
        out.pushBack("-"s);
    } else if (startWith(arg, "--"s) == Match::PARTIAL) {
        out.emplaceBack(Token::OPTION, next(arg, 2));
    } else if (startWith(arg, "-"s) == Match::PARTIAL) {
        Str flags = next(arg, 1);
        for (auto r : iterRunes(flags))
            out.emplaceBack(r);
    } else {
        out.pushBack(arg);
    }
}

export void tokenize(Slice<Str> args, Vec<Token>& out) {
    for (auto& arg : args)
        tokenize(arg, out);
}

export void tokenize(int argc, char** argv, Vec<Token>& out) {
    for (int i = 0; i < argc; ++i)
        tokenize(Str::fromNullterminated(argv[i]), out);
}

// MARK: Values ----------------------------------------------------------------

export template <typename T>
struct ValueParser;

export template <>
struct ValueParser<bool> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("true|false"s);
    }

    static Res<bool> parse(Cursor<Token>&) {
        return Ok(true);
    }
};

export template <>
struct ValueParser<isize> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("integer"s);
    }

    static Res<isize> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::other("missing value");

        auto value = c.next().value;

        auto result = Io::atoi(value);
        if (not result)
            return Error::other("expected integer");

        return Ok(result.unwrap());
    }
};

export template <>
struct ValueParser<Str> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("string"s);
    }

    static Res<Str> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::other("missing value");

        return Ok(c.next().value);
    }
};

export template <typename T>
struct ValueParser<Vec<T>> {
    static Res<> usage(Io::TextWriter& w) {
        ValueParser<T>::usage(w);
        return w.writeStr("..."s);
    }

    static Res<Vec<T>> parse(Cursor<Token>& c) {
        Vec<T> values;
        while (not c.ended() and c->kind == Token::OPERAND)
            values.pushBack(try$(ValueParser<T>::parse(c)));
        return Ok(values);
    }
};

export template <typename T>
struct ValueParser<Opt<T>> {
    static Res<> usage(Io::TextWriter& w) {
        return ValueParser<T>::usage(w);
    }

    static Res<Opt<T>> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Ok(NONE);

        return ValueParser<T>::parse(c);
    }
};

// MARK: Options ---------------------------------------------------------------

export enum struct OptionKind {
    OPTION,
    OPERAND,
    EXTRA,
};

export struct _OptionImpl {
    OptionKind kind;
    Opt<Rune> shortName;
    String longName;
    String description;

    _OptionImpl(
        OptionKind kind,
        Opt<Rune> shortName,
        String longName,
        String description
    ) : kind(kind),
        shortName(shortName),
        longName(std::move(longName)),
        description(std::move(description)) {}

    virtual ~_OptionImpl() = default;

    virtual Res<> eval(Cursor<Token>& c) = 0;

    virtual Res<> usage(Io::TextWriter& w) {
        if (kind == OptionKind::OPTION) {
            try$(w.writeRune('['));
            if (shortName)
                try$(format(w, "-{c},", shortName.unwrap()));
            try$(format(w, "--{}", longName));
            try$(w.writeRune(']'));
        } else if (kind == OptionKind::OPERAND) {
            try$(w.writeStr(longName.str()));
        } else if (kind == OptionKind::EXTRA) {
            try$(format(w, "[-- {}...]", longName));
        }

        return Ok();
    }
};

export template <typename T>
struct OptionImpl : _OptionImpl {
    Opt<T> value;

    OptionImpl(
        OptionKind kind,
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue
    ) : _OptionImpl(kind, shortName, std::move(longName), std::move(description)),
        value(defaultValue) {}

    Res<> eval(Cursor<Token>& c) override {
        value = try$(ValueParser<T>::parse(c));
        return Ok();
    }
};

export template <typename T>
struct Option {
    Rc<OptionImpl<T>> _impl;

    Option(Rc<OptionImpl<T>> impl)
        : _impl(std::move(impl)) {}

    T const& unwrap() const {
        return _impl->value.unwrap();
    }

    operator T() const {
        return _impl->value.unwrapOr(T{});
    }

    operator Rc<_OptionImpl>() {
        return _impl;
    }
};

export using Flag = Option<bool>;

export Flag flag(Opt<Rune> shortName, String longName, String description) {
    return makeRc<OptionImpl<bool>>(OptionKind::OPTION, shortName, longName, description, false);
}

export template <typename T>
Option<T> option(Opt<Rune> shortName, String longName, String description, Opt<T> defaultValue = NONE) {
    return makeRc<OptionImpl<T>>(OptionKind::OPTION, shortName, longName, description, defaultValue);
}

export template <typename T>
Option<T> operand(String longName, String description, T defaultValue = {}) {
    return makeRc<OptionImpl<T>>(OptionKind::OPERAND, NONE, longName, description, defaultValue);
}

export Option<Vec<Str>> extra(String description) {
    return makeRc<OptionImpl<Vec<Str>>>(OptionKind::EXTRA, NONE, ""s, description, Vec<Str>{});
}

// MARK: Command ---------------------------------------------------------------

export struct Command : Meta::Pinned {
    using Callback = Func<Async::Task<>(Sys::Context&)>;

    String longName;
    String description = ""s;
    Vec<Rc<_OptionImpl>> options;
    Opt<Callback> callbackAsync;

    Vec<Rc<Command>> _commands;
    Command* _parent = nullptr;

    Option<bool> _help = flag('h', "help"s, "Show this help message and exit."s);
    Option<bool> _usage = flag('u', "usage"s, "Show usage message and exit."s);
    Option<bool> _version = flag('v', "version"s, "Show version information and exit."s);
    Option<Vec<Str>> _debug = option<Vec<Str>>(NONE, "debug"s, "Enable or list debug flags"s, Vec<Str>{});

    bool _invoked = false;

    Command(
        String longName,
        String description = ""s,
        Vec<Rc<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    )
        : longName(std::move(longName)),
          description(std::move(description)),
          options(std::move(options)),
          callbackAsync(std::move(callbackAsync)) {
        options.pushBack(_help);
        options.pushBack(_usage);
        options.pushBack(_version);
        options.pushBack(_debug);
    }

    Command& subCommand(
        String longName,
        String description = ""s,
        Vec<Rc<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    ) {
        auto cmd = makeRc<Command>(
            longName,
            description,
            options,
            std::move(callbackAsync)
        );
        cmd->_parent = this;
        _commands.pushBack(cmd);
        return *last(_commands);
    }

    template <typename T>
    void option(Option<T> field) {
        options.pushBack(field._impl);
    }

    template <typename T>
    Option<T> option(
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue = NONE
    ) {
        auto store = makeRc<OptionImpl<T>>(
            OptionKind::OPTION,
            shortName,
            longName,
            description,
            defaultValue
        );
        options.pushBack(store);
        return {store};
    }

    Flag flag(Opt<Rune> shortName, String longName, String description) {
        auto store = makeRc<OptionImpl<bool>>(
            OptionKind::OPTION,
            shortName,
            longName,
            description,
            false
        );
        options.pushBack(store);
        return {store};
    }

    Res<> usage(Io::TextWriter& w) {
        auto printLongName = [](this auto& self, Command& cmd, Io::TextWriter& w) -> Res<> {
            if (cmd._parent)
                try$(self(*cmd._parent, w));
            try$(format(w, "{} ", cmd.longName));
            return Ok();
        };

        try$(printLongName(*this, w));

        for (auto& opt : options) {
            if (opt->kind != OptionKind::OPTION)
                continue;
            try$(opt->usage(w));
            try$(w.writeRune(' '));
        }

        for (auto& opt : options) {
            if (opt->kind != OptionKind::OPERAND)
                continue;
            try$(opt->usage(w));
            try$(w.writeRune(' '));
        }

        for (auto& opt : options) {
            if (opt->kind != OptionKind::EXTRA)
                continue;
            try$(opt->usage(w));
        }

        if (Karm::any(_commands)) {
            try$(format(w, "<command> [args...]"));
        }

        return Ok();
    }

    Res<> _showUsage(Io::TextWriter& w) {
        try$(format(w, "Usage: "));
        try$(usage(w));
        try$(w.writeRune('\n'));

        return Ok();
    }

    Res<> _showHelp(Io::TextWriter& w) {
        try$(format(w, "Usage:\n  "));
        try$(usage(w));
        try$(w.writeStr("\n\n"s));

        try$(format(w, "Description:\n  {}\n\n", description));

        if (Karm::any(options)) {
            try$(w.writeStr("Options:\n"s));
            for (auto& opt : options) {
                if (opt->kind != OptionKind::OPTION)
                    continue;

                try$(w.writeStr("  "s));
                if (opt->shortName)
                    try$(format(w, "-{c}, ", opt->shortName.unwrap()));

                try$(format(w, "--{} - {}\n", opt->longName, opt->description));
            }
            try$(w.writeRune('\n'));
        }

        if (Karm::any(_commands)) {
            try$(w.writeStr("Subcommands:\n"s));
            for (auto& cmd : _commands) {
                try$(format(w, "  {} - {}\n", cmd->longName, cmd->description));
            }
            try$(w.writeRune('\n'));
        }

        return Ok();
    }

    Res<bool> _evalOption(Cursor<Token>& c) {
        bool found = false;

        for (auto& opt : options) {
            if (c.ended())
                break;

            if (opt->kind != OptionKind::OPTION)
                continue;

            bool shortNameMatch = opt->shortName and c->flag == opt->shortName.unwrap();
            bool longNameMatch = c->value == opt->longName;

            if (not(shortNameMatch or longNameMatch))
                continue;

            c.next();
            try$(opt->eval(c));
            found = true;
        }

        return Ok(found);
    }

    Res<> _evalParams(Cursor<Token>& c) {
        for (auto& opt : options) {
            while (try$(_evalOption(c)))
                ;

            if (c.ended())
                break;

            if (opt->kind != OptionKind::OPERAND)
                continue;

            try$(opt->eval(c));
        }

        return Ok();
    }

    Async::Task<> execAsync(Sys::Context& ctx) {
        auto args = Sys::useArgs(ctx);
        Vec<Token> tokens;
        for (usize i = 0; i < args.len(); ++i)
            tokenize(args[i], tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context& ctx, Slice<Str> args) {
        Vec<Token> tokens;
        tokenize(args, tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context& ctx, Cursor<Token> c) {
        co_try$(_evalParams(c));

        if (_help) {
            co_try$(_showHelp(Sys::out()));
            co_return Ok();
        }

        if (_usage) {
            co_try$(_showUsage(Sys::out()));
            co_return Ok();
        }

        if (_version) {
            co_try$(format(Sys::out(), "{} {}\n", longName, stringify$(__ck_version_value) ""s));
            co_return Ok();
        }

        if (_debug.unwrap().len()) {
            auto const& list = _debug.unwrap();
            if (list.len() == 1 and list[0] == "list") {
                co_try$(format(Sys::out(), "{}\n", Debug::flags()));
                co_return Ok();
            }

            for (auto const& l : list) {
                co_try$(Debug::enable(l));
            }
        }

        _invoked = true;
        if (callbackAsync)
            co_trya$(callbackAsync.unwrap()(ctx));

        if (Karm::any(_commands) and c.ended()) {
            co_try$(_showUsage(Sys::out()));
            co_return Error::invalidInput("expected subcommand");
        }

        if (not c.ended()) {
            if (c->kind != Token::OPERAND)
                co_return Error::invalidInput("expected subcommand");

            if (not Karm::any(_commands))
                co_return Error::invalidInput("unexpected subcommand");

            auto value = c->value;
            c.next();

            for (auto& cmd : _commands) {
                if (value != cmd->longName)
                    continue;
                co_return co_await cmd->execAsync(ctx, c);
            }

            co_return Error::invalidInput("unknown subcommand");
        }
        co_return Ok();
    }

    operator bool() const {
        return _invoked;
    }
};

} // namespace Karm::Cli

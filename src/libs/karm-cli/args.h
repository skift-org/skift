#pragma once

#include <karm-base/func.h>
#include <karm-base/opt.h>
#include <karm-base/rc.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-sys/context.h>

namespace Karm::Cli {

// MARK: Tokenizer -------------------------------------------------------------

struct Token {
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
    Str value;

    Token(Str value)
        : Token(Kind::OPERAND, value) {}

    Token(Kind kind, Str value)
        : kind(kind), value(value) {}

    Token(Kind kind)
        : kind(kind), value("") {}

    Token(Rune flag)
        : kind(Kind::FLAG), flag(flag), value("") {}

    bool operator==(Token const &other) const = default;

    void repr(Io::Emit &e) const {
        if (kind == Kind::FLAG)
            e("(token {} {#c})", kind, flag);
        else
            e("(token {} {#})", kind, value);
    }
};

void tokenize(Str arg, Vec<Token> &out);

void tokenize(Slice<Str> args, Vec<Token> &out);

void tokenize(int argc, char **argv, Vec<Token> &out);

// MARK: Values ----------------------------------------------------------------

template <typename T>
struct ValueParser;

template <>
struct ValueParser<bool> {
    static void usage(Io::Emit &e);

    static Res<bool> parse(Cursor<Token> &);
};

template <>
struct ValueParser<i32> {
    static void usage(Io::Emit &e);

    static Res<i32> parse(Cursor<Token> &c);
};

template <>
struct ValueParser<Str> {
    static void usage(Io::Emit &e);

    static Res<Str> parse(Cursor<Token> &c);
};

template <typename T>
struct ValueParser<Vec<T>> {
    static void usage(Io::Emit &e) {
        ValueParser<T>::usage(e);
        e("...");
    }

    static Res<Vec<T>> parse(Cursor<Token> &c) {
        Vec<T> values;

        while (not c.ended() and c->kind == Token::OPERAND) {
            values.pushBack(try$(ValueParser<T>::parse(c)));
        }

        return Ok(values);
    }
};

// MARK: Options ---------------------------------------------------------------

enum struct OptionKind {
    OPTION,
    OPERAND,
    EXTRA,
};

struct _OptionImpl {
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

    virtual Res<> eval(Cursor<Token> &c) = 0;
};

template <typename T>
struct OptionImpl : public _OptionImpl {
    Opt<T> value;

    OptionImpl(
        OptionKind kind,
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue
    ) : _OptionImpl(kind, shortName, std::move(longName), std::move(description)),
        value(defaultValue) {}

    void usage(Io::Emit &e) {
        if (kind == OptionKind::OPTION) {
            if (shortName)
                e("-{}", shortName.unwrap());
            else
                e("    ");
            e(", --{}", longName);
        } else if (kind == OptionKind::OPERAND) {
            e("{}", longName);
        } else if (kind == OptionKind::EXTRA) {
            e("[{}...]", longName);
        }
    }

    Res<> eval(Cursor<Token> &c) override {
        value = try$(ValueParser<T>::parse(c));
        return Ok();
    }
};

template <typename T>
struct Option {
    Strong<OptionImpl<T>> _impl;

    Option(Strong<OptionImpl<T>> impl)
        : _impl(std::move(impl)) {}

    T const &unwrap() const {
        return _impl->value.unwrap();
    }

    operator T() const {
        return _impl->value.unwrapOr(T{});
    }

    operator Strong<_OptionImpl>() {
        return _impl;
    }
};

using Flag = Option<bool>;

static inline Flag flag(Opt<Rune> shortName, String longName, String description) {
    return makeStrong<OptionImpl<bool>>(OptionKind::OPTION, shortName, longName, description, false);
}

template <typename T>
static inline Option<T> option(Opt<Rune> shortName, String longName, String description, Opt<T> defaultValue = NONE) {
    return makeStrong<OptionImpl<T>>(OptionKind::OPTION, shortName, longName, description, defaultValue);
}

template <typename T>
static inline Option<T> operand(String longName, String description, T defaultValue = {}) {
    return makeStrong<OptionImpl<T>>(OptionKind::OPERAND, NONE, longName, description, defaultValue);
}

static inline Option<Vec<Str>> extra(String description) {
    return makeStrong<OptionImpl<Vec<Str>>>(OptionKind::EXTRA, NONE, ""s, description, Vec<Str>{});
}

// MARK: Command ---------------------------------------------------------------

struct Command {
    using Callback = Func<Async::Task<>(Sys::Context &)>;

    struct Props {
    };

    String longName;
    Opt<Rune> shortName;
    String description = ""s;
    Vec<Strong<_OptionImpl>> options;
    Opt<Callback> callbackAsync;

    Vec<Strong<Command>> _commands;

    Option<bool> _help = flag('h', "help"s, "Show this help message and exit."s);
    Option<bool> _usage = flag('u', "usage"s, "Show usage message and exit."s);

    bool _invoked = false;

    Command(
        String longName,
        Opt<Rune> shortName = NONE,
        String description = ""s,
        Vec<Strong<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    )
        : longName(std::move(longName)),
          shortName(shortName),
          description(std::move(description)),
          options(std::move(options)),
          callbackAsync(std::move(callbackAsync)) {
        options.pushBack(_help);
        options.pushBack(_usage);
    }

    Command &subCommand(
        String longName,
        Opt<Rune> shortName = NONE,
        String description = ""s,
        Vec<Strong<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    ) {
        auto cmd = makeStrong<Command>(
            longName,
            shortName,
            description,
            options,
            std::move(callbackAsync)
        );
        _commands.pushBack(cmd);
        return *cmd;
    }

    template <typename T>
    Option<T> option(
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue = NONE
    ) {
        auto store = makeStrong<OptionImpl<T>>(
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
        auto store = makeStrong<OptionImpl<bool>>(
            OptionKind::OPTION,
            shortName,
            longName,
            description,
            false
        );
        options.pushBack(store);
        return {store};
    }

    template <typename T>
    void option(Option<T> field) {
        options.pushBack(field._impl);
    }

    Res<> _showHelp() {
        return Ok();
    }

    Res<> _showUsage() {
        return Ok();
    }

    Res<bool> _evalOption(Cursor<Token> &c) {
        bool found = false;

        for (auto &opt : options) {
            if (c.ended())
                break;

            if (opt->kind != OptionKind::OPTION)
                continue;

            try$(opt->eval(c));
            found = true;
        }

        return Ok(found);
    }

    Res<> _evalParams(Cursor<Token> &c) {
        while (not c.ended()) {
            c.next();
        }

        return Ok();
    }

    Async::Task<> execAsync(Sys::Context &ctx) {
        auto args = Sys::useArgs(ctx);
        Vec<Token> tokens;
        for (usize i = 0; i < args.len(); ++i)
            tokenize(args[i], tokens);
        return execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context &ctx, Slice<Str> args) {
        Vec<Token> tokens;
        tokenize(args, tokens);
        return execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context &ctx, Slice<Token> tokens) {
        Cursor<Token> c = tokens;
        co_try$(_evalParams(c));

        if (_help) {
            co_try$(_showHelp());
            co_return Ok();
        }

        if (_usage) {
            co_try$(_showUsage());
            co_return Ok();
        }

        _invoked = true;
        if (callbackAsync)
            co_trya$(callbackAsync.unwrap()(ctx));

        for (auto &cmd : _commands)
            co_trya$(cmd->execAsync(ctx));

        co_return Ok();
    }

    operator bool() const {
        return _invoked;
    }
};

} // namespace Karm::Cli

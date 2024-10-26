#pragma once

#include <karm-base/func.h>
#include <karm-base/opt.h>
#include <karm-base/rc.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>
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
    Str value = "";

    Token(Str value)
        : Token(Kind::OPERAND, value) {}

    Token(Kind kind, Str value)
        : kind(kind), value(value) {}

    Token(Kind kind)
        : kind(kind) {}

    Token(Rune flag)
        : kind(Kind::FLAG), flag(flag) {}

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
struct ValueParser<isize> {
    static void usage(Io::Emit &e);

    static Res<isize> parse(Cursor<Token> &c);
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

template <typename T>
struct ValueParser<Opt<T>> {
    static void usage(Io::Emit &e) {
        ValueParser<T>::usage(e);
    }

    static Res<Opt<T>> parse(Cursor<Token> &c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Ok(NONE);

        return ValueParser<T>::parse(c);
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

    virtual void usage(Io::Emit &e) {
        if (kind == OptionKind::OPTION) {
            e("[");
            if (shortName)
                e("-{c},", shortName.unwrap());
            e("--{}", longName);
            e("]");
        } else if (kind == OptionKind::OPERAND) {
            e("{}", longName);
        } else if (kind == OptionKind::EXTRA) {
            e("[-- {}...]", longName);
        }
    }
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

    void usage(Io::Emit &e) {
        e("{} ", longName);

        for (auto &opt : options) {
            if (opt->kind != OptionKind::OPTION)
                continue;
            opt->usage(e);
            e(" ");
        }

        for (auto &opt : options) {
            if (opt->kind != OptionKind::OPERAND)
                continue;
            opt->usage(e);
            e(" ");
        }

        for (auto &opt : options) {
            if (opt->kind != OptionKind::EXTRA)
                continue;
            opt->usage(e);
        }

        if (any(_commands)) {
            e("<command> [args...]");
        }
    }

    void _showUsage(Io::Emit &e) {
        e("Usage: ");
        usage(e);
        e("\n");
    }

    void _showHelp(Io::Emit &e) {
        e("{}\n\n", longName);

        e("Usage:\n\t");
        usage(e);
        e("\n\n");

        e("Description:\n\t{}\n\n", description);

        if (any(options)) {
            e("Options:\n");
            for (auto &opt : options) {
                if (opt->kind != OptionKind::OPTION)
                    continue;

                e("\t");
                if (opt->shortName)
                    e("-{c}, ", opt->shortName.unwrap());

                e("--{} - {}\n", opt->longName, opt->description);
            }
            e("\n");
        }

        if (any(_commands)) {
            e("Subcommands:\n");
            for (auto &cmd : _commands) {
                e("\t{c} {} - {}\n", cmd->shortName.unwrapOr(' '), cmd->longName, cmd->description);
            }
        }
    }

    Res<bool> _evalOption(Cursor<Token> &c) {
        bool found = false;

        for (auto &opt : options) {
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

    Res<> _evalParams(Cursor<Token> &c) {
        for (auto &opt : options) {
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

    Async::Task<> execAsync(Sys::Context &ctx) {
        auto args = Sys::useArgs(ctx);
        Vec<Token> tokens;
        for (usize i = 0; i < args.len(); ++i)
            tokenize(args[i], tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context &ctx, Slice<Str> args) {
        Vec<Token> tokens;
        tokenize(args, tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context &ctx, Cursor<Token> c) {
        co_try$(_evalParams(c));

        if (_help) {
            Io::Emit e{Sys::out()};
            _showHelp(e);
            co_return Ok();
        }

        if (_usage) {
            Io::Emit e{Sys::out()};
            _showUsage(e);
            co_return Ok();
        }

        _invoked = true;
        if (callbackAsync)
            co_trya$(callbackAsync.unwrap()(ctx));

        if (not c.ended()) {
            if (c->kind != Token::OPERAND)
                co_return Error::invalidInput("expected subcommand");

            if (not any(_commands))
                co_return Error::invalidInput("unexpected subcommand");

            auto value = c->value;
            c.next();

            for (auto &cmd : _commands) {

                bool shortNameMatch = value.len() == 1 and iterRunes(value).first() == cmd->shortName;
                bool longNameMatch = value == cmd->longName;

                if (not(shortNameMatch or longNameMatch))
                    continue;

                co_return co_await cmd->execAsync(ctx, c);
            }

            logError("unknown subcommand '{}'", value);
            co_return Error::invalidInput("unknown subcommand");
        }
        co_return Ok();
    }

    operator bool() const {
        return _invoked;
    }
};

} // namespace Karm::Cli

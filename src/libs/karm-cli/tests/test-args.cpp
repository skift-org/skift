#include <karm-cli/args.h>
#include <karm-test/macros.h>

namespace Karm::Cli::Tests {

test$("karm-cli-args-tokenizer") {
    Array args = {
        "ls"s,
        "-h"s,
        "--help"s,
        "-la"s,
        "/usr/bin"s,
    };

    Vec<Token> tokens;
    tokenize(args, tokens);

    expectEq$(tokens.len(), 6uz);

    expectEq$(tokens[0].kind, Token::OPERAND);
    expectEq$(tokens[0].value, "ls"s);

    expectEq$(tokens[1].kind, Token::FLAG);
    expectEq$(tokens[1].flag, (Rune)'h');

    expectEq$(tokens[2].kind, Token::OPTION);
    expectEq$(tokens[2].value, "help"s);

    expectEq$(tokens[3].kind, Token::FLAG);
    expectEq$(tokens[3].flag, (Rune)'l');

    expectEq$(tokens[4].kind, Token::FLAG);
    expectEq$(tokens[4].flag, (Rune)'a');

    expectEq$(tokens[5].kind, Token::OPERAND);
    expectEq$(tokens[5].value, "/usr/bin"s);

    return Ok();
}

testAsync$("karm-cli-args-simple-command") {
    Command cmd{
        "test"s,
        NONE,
    };

    Vec<Str> args = {};

    co_trya$(cmd.execAsync(Sys::globalContext(), args));

    if (not cmd)
        co_return Error::other("command not invoked");

    co_return Ok();
}

testAsync$("karm-cli-args-nested-command") {
    Command cmd{
        "test"s,
        NONE,
    };

    auto& subCmd = cmd.subCommand("sub"s);

    Array<Str, 1> args = {"sub"s};

    co_trya$(cmd.execAsync(Sys::globalContext(), args));

    if (not cmd)
        co_return Error::other("command not invoked");

    if (not subCmd)
        co_return Error::other("sub-command not invoked");

    co_return Ok();
}

} // namespace Karm::Cli::Tests

#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-html/builder.h>
#include <web-html/lexer.h>

Res<> entryPoint(Sys::Ctx &) {
    auto file = try$(Sys::File::open("bundle://web-html-cli/exemple.html"_url));

    auto buf = try$(Io::readAllUtf8(file));

    Sys::println("Orginal Source:");
    Sys::println("{}", buf);

    Sys::println("Lexing:");
    Web::Html::Lexer lexer;
    for (auto r : iterRunes(buf))
        lexer.consume(r);

    lexer.consume(-1, true);
    Sys::println("\t(no output if successful)");

    Sys::println("Tokens:");
    for (auto const &t : lexer.tokens())
        Sys::println("{}", t);

    Sys::println("Building:");
    Web::Html::Builder builder;
    for (auto const &t : lexer.tokens())
        builder.accept(t);

    return Ok();
}

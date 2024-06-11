#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-html/lexer.h>
#include <web-html/parser.h>

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: web-html.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "dump-dom") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        auto doc = makeStrong<Web::Dom::Document>();
        Web::Html::Parser parser{doc};
        parser.write(buf);
        Sys::println("{}", doc);

        co_return Ok();
    } else if (verb == "dump-tokens") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        Vec<Web::Html::Token> tokens;
        struct VecSink : public Web::Html::Sink {
            Vec<Web::Html::Token> &tokens;
            VecSink(Vec<Web::Html::Token> &tokens)
                : tokens(tokens) {
            }
            void accept(Web::Html::Token const &token) override {
                tokens.pushBack(token);
            }
        };

        VecSink sink{tokens};
        Web::Html::Lexer lexer{};
        lexer.bind(sink);

        for (auto r : iterRunes(buf))
            lexer.consume(r);

        for (auto &t : tokens)
            Sys::println("{}", t);

        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-dom, dump-tokens)\n", verb);
        co_return Error::invalidInput();
    }
}

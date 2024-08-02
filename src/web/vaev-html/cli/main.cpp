#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <vaev-html/lexer.h>
#include <vaev-html/parser.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: vaev-html.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "dump-dom") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        auto doc = makeStrong<Vaev::Dom::Document>();
        Vaev::Html::Parser parser{doc};
        parser.write(buf);
        Sys::println("{}", doc);

        co_return Ok();
    } else if (verb == "dump-tokens") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        Vec<Vaev::Html::Token> tokens;

        struct VecSink : public Vaev::Html::Sink {
            Vec<Vaev::Html::Token> &tokens;

            VecSink(Vec<Vaev::Html::Token> &tokens)
                : tokens(tokens) {
            }

            void accept(Vaev::Html::Token const &token) override {
                tokens.pushBack(token);
            }
        };

        VecSink sink{tokens};
        Vaev::Html::Lexer lexer{};
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

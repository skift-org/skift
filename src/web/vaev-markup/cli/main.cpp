#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: vaev-markup.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "html-dump-dom") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        auto doc = makeStrong<Vaev::Markup::Document>();
        Vaev::Markup::HtmlParser parser{doc};
        parser.write(buf);
        Sys::println("{}", doc);

        co_return Ok();
    } else if (verb == "html-dump-tokens") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        Vec<Vaev::Markup::HtmlToken> tokens;

        struct VecSink : public Vaev::Markup::HtmlSink {
            Vec<Vaev::Markup::HtmlToken> &tokens;

            VecSink(Vec<Vaev::Markup::HtmlToken> &tokens)
                : tokens(tokens) {
            }

            void accept(Vaev::Markup::HtmlToken const &token) override {
                tokens.pushBack(token);
            }
        };

        VecSink sink{tokens};
        Vaev::Markup::HtmlLexer lexer{};
        lexer.bind(sink);

        for (auto r : iterRunes(buf))
            lexer.consume(r);

        for (auto &t : tokens)
            Sys::println("{}", t);

        co_return Ok();
    } else if (verb == "xml-dump-dom") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        auto start = Sys::now();

        Vaev::Markup::XmlParser parser{};
        Io::SScan s{buf};
        auto res = parser.parse(s, Vaev::HTML);

        auto elapsed = Sys::now() - start;
        logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

        Sys::println("{}", res);

        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: html-dump-dom, html-dump-tokens, xml-dump-dom)\n", verb);
        co_return Error::invalidInput();
    }
}

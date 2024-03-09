#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-html/tokenizer.h>

Res<> entryPoint(Sys::Ctx &) {
    auto file = try$(Sys::File::open("bundle://web-html-cli/index.html"_url));

    auto buf = try$(Io::readAllUtf8(file));

    // Sys::println("Orginal Source:");
    // Sys::println("{}", buf);

    struct : public Web::Html::Sink {
        void accept(Web::Html::Token const &token) override {
            Sys::println("{}", token);
        }
    } sink;

    Sys::println("Tokens:");
    Web::Html::Tokenizer tokenizer{sink};

    for (auto r : iterRunes(buf))
        tokenizer.consume(r);

    tokenizer.consume(-1, true);

    return Ok();
}

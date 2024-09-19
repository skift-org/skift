#include <karm-cli/args.h>
#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-print/pdf.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <karm-ui/app.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/render.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>

#include "inspector.h"

namespace Vaev::Tools {

Res<> cssDumpStylesheet(Mime::Url const &url) {
    auto start = Sys::now();
    auto stylesheet = try$(Vaev::Driver::fetchStylesheet(url));
    auto elapsed = Sys::now() - start;
    logInfo("fetched in {}ms", elapsed.toUSecs() / 1000.0);
    Sys::println("{#}", stylesheet);
    return Ok();
}

Res<> cssDumpSst(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto start = Sys::now();

    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    Vaev::Css::Sst sst = Vaev::Css::consumeRuleList(lex, true);

    auto elapsed = Sys::now() - start;
    logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

    Sys::println("{}", sst);

    return Ok();
}

Res<> cssDumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    while (not lex.ended())
        Sys::println("{}", lex.next());
    return Ok();
}

Res<> styleListProps() {
    Vaev::Style::StyleProp::any([]<typename T>(Meta::Type<T>) {
        Sys::println("{}", T::name());
        return false;
    });
    return Ok();
}

Res<> markupDumpDom(Mime::Url const &url) {
    auto dom = try$(Vaev::Driver::fetchDocument(url));
    Sys::println("{}", dom);
    return Ok();
}

Res<> markupDumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

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

    return Ok();
}

Vaev::Style::Media constructMedia(Print::PaperStock paper) {
    return {
        .type = Vaev::MediaType::SCREEN,
        .width = Vaev::Px{paper.width},
        .height = Vaev::Px{paper.height},
        .aspectRatio = paper.width / paper.height,
        .orientation = Vaev::Orientation::LANDSCAPE,

        .resolution = Vaev::Resolution::fromDpi(96),
        .scan = Vaev::Scan::PROGRESSIVE,
        .grid = false,
        .update = Vaev::Update::NONE,

        .overflowBlock = Vaev::OverflowBlock::PAGED,
        .overflowInline = Vaev::OverflowInline::NONE,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = Vaev::ColorGamut::SRGB,
        .pointer = Vaev::Pointer::NONE,
        .hover = Vaev::Hover::NONE,
        .anyPointer = Vaev::Pointer::NONE,
        .anyHover = Vaev::Hover::NONE,

        .prefersReducedMotion = Vaev::ReducedMotion::REDUCE,
        .prefersReducedTransparency = Vaev::ReducedTransparency::REDUCE,
        .prefersContrast = Vaev::Contrast::MORE,
        .forcedColors = Vaev::Colors::NONE,
        .prefersColorScheme = Vaev::ColorScheme::LIGHT,
        .prefersReducedData = Vaev::ReducedData::NO_PREFERENCE,
    };
}

struct Html2PdfOption {
    bool dumpStyle = false;
    bool dumpDom = false;
    bool dumpLayout = false;
    bool dumpPaint = false;
};

Res<> html2pdf(Mime::Url const &input, Io::Writer &output, Html2PdfOption options = {}) {
    auto start = Sys::now();

    auto dom = try$(Vaev::Driver::fetchDocument(input));
    auto paper = Print::A4;
    auto media = constructMedia(paper);
    auto [style, layout, paint] = Vaev::Driver::render(*dom, media, paper);
    auto elapsed = Sys::now() - start;

    logInfo("render time: {}", elapsed);

    if (options.dumpDom)
        Sys::println("--- START OF DOM ---\n{}\n--- END OF DOM ---\n", dom);

    if (options.dumpStyle)
        Sys::println("--- START OF STYLE ---\n{}\n--- END OF STYLE ---\n", style);

    if (options.dumpLayout)
        Sys::println("--- START OF LAYOUT ---\n{}\n--- END OF LAYOUT ---\n", layout);

    if (options.dumpPaint)
        Sys::println("--- START OF PAINT ---\n{}\n--- END OF PAINT ---\n", paint);

    Print::PdfPrinter printer{Print::A4, Print::Density::DEFAULT};
    paint->print(printer);

    Io::TextEncoder<> encoder{output};
    Io::Emit e{encoder};
    printer.write(e);
    try$(e.flush());

    return Ok();
}

} // namespace Vaev::Tools

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto inputArg = Cli::operand<Str>("input"s, "Input file"s, ""s);
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file"s, "-"s);

    Cli::Command cmd{
#ifdef __ck_odoo__
        "paper-munch"s,
#else
        "vaev-tools"s,
#endif
        NONE,
        "A next generation document generation tool"s,
    };

    Cli::Command &cssCmd = cmd.subCommand(
        "css"s,
        'c',
        "CSS related commands"s
    );

    cssCmd.subCommand(
        "dump-stylesheet"s,
        's',
        "Dump a stylesheet"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpStylesheet(input);
        }
    );

    cssCmd.subCommand(
        "dump-sst"s,
        'a',
        "Dump a stylesheet"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpSst(input);
        }
    );

    cssCmd.subCommand(
        "dump-tokens"s,
        't',
        "Dump CSS tokens"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpTokens(input);
        }
    );

    Cli::Command &styleCmd = cmd.subCommand(
        "style"s,
        's',
        "Style related commands"s
    );

    styleCmd.subCommand(
        "list-props"s,
        'l',
        "List all style properties"s,
        {},
        [](Sys::Context &) -> Async::Task<> {
            co_return Vaev::Tools::styleListProps();
        }
    );

    Cli::Command &markupCmd = cmd.subCommand(
        "markup"s,
        'm',
        "Markup related commands"s
    );

    markupCmd.subCommand(
        "dump-dom"s,
        NONE,
        "Dump the DOM tree"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::markupDumpDom(input);
        }
    );

    markupCmd.subCommand(
        "dump-tokens"s,
        't',
        "Dump HTML tokens"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::markupDumpTokens(input);
        }
    );

    Cli::Flag dumpStyleArg = Cli::flag('s', "dump-style"s, "Dump the stylesheet"s);
    Cli::Flag dumpDomArg = Cli::flag('d', "dump-dom"s, "Dump the DOM tree"s);
    Cli::Flag dumpLayoutArg = Cli::flag('l', "dump-layout"s, "Dump the layout tree"s);
    Cli::Flag dumpPaintArg = Cli::flag('p', "dump-paint"s, "Dump the paint tree"s);

    cmd.subCommand(
        "html2pdf"s,
        'r',
        "Convert HTML to PDF"s,
        {inputArg, outputArg, dumpStyleArg, dumpDomArg, dumpLayoutArg, dumpPaintArg},
        [inputArg, outputArg, dumpStyleArg, dumpDomArg, dumpLayoutArg, dumpPaintArg](Sys::Context &) -> Async::Task<> {
            Vaev::Tools::Html2PdfOption options{
                .dumpStyle = dumpStyleArg,
                .dumpDom = dumpDomArg,
                .dumpLayout = dumpLayoutArg,
                .dumpPaint = dumpPaintArg,
            };

            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            if (outputArg.unwrap() == "-"s)
                co_return Vaev::Tools::html2pdf(input, Sys::out(), options);

            auto outputUrl = co_try$(Mime::parseUrlOrPath(outputArg));
            auto outputFile = co_try$(Sys::File::create(outputUrl));
            co_return Vaev::Tools::html2pdf(input, outputFile, options);
        }
    );

    auto &inspectorCmd = cmd.subCommand(
        "inspector"s,
        'i',
        "View a document in the inspector for debugging"s,
        {inputArg}
    );

    Ui::mountApp(inspectorCmd, [inputArg] -> Ui::Child {
        auto input = Mime::parseUrlOrPath(inputArg).unwrap();
        auto dom = Vaev::Driver::fetchDocument(input);
        return Vaev::Tools::inspector(input, dom);
    });

    co_return co_await cmd.execAsync(ctx);
}

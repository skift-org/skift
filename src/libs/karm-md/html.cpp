module;

#include <karm-logger/logger.h>

export module Karm.Md:html;

import :base;

namespace Karm::Md {

String _htmlEscape(Str str) {
    StringBuilder sw;
    sw.ensure(str.len());
    for (auto r : iterRunes(str)) {
        if (r == '&')
            sw.append("&amp;"s);
        else if (r == '<')
            sw.append("&lt;"s);
        else if (r == '>')
            sw.append("&gt;"s);
        else if (r == '"')
            sw.append("&quot;"s);
        else if (r == '\"')
            sw.append("&#39;"s);
        else
            sw.append(r);
    }
    return sw.take();
}

void _renderSpan(Node const& node, Io::Emit& e) {
    node.visit(Visitor{
        [&](String const& s) {
            e(s);
        },
        [&](auto const& n) {
            logWarn("could not render {} as a span", n);
        },
    });
}

void _renderParagraphContent(Paragraph const& p, Io::Emit& e) {
    for (auto const& n : p)
        _renderSpan(n, e);
}

void _renderBlock(Node const& node, Io::Emit& e) {
    node.visit(Visitor{
        [&](Heading const& h) {
            e("<h{}>", h.level);
            _renderParagraphContent(h.children, e);
            e("</h{}>", h.level);
        },
        [&](Paragraph const& p) {
            e("<p>");
            _renderParagraphContent(p, e);
            e("</p>");
        },
        [&](Code const& p) {
            e("<pre><code>{}</code></pre>", _htmlEscape(p.text));
        },
        [&](Hr const&) {
            e("<hr/>");
        },
        [&](auto const& n) {
            logWarn("could not render {} as a block", n);
        },
    });
}

export void renderHtml(Document const& doc, Io::Emit& e) {
    e(R"(<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"></head><body>)");
    for (auto const& n : doc.children)
        _renderBlock(n, e);
    e(R"(</body></html>)");
}

export String renderHtml(Document const& doc) {
    Io::StringWriter sw;
    Io::Emit e{sw};
    renderHtml(doc, e);
    return sw.take();
}

} // namespace Karm::Md
#include <karm-test/macros.h>
#include <web-html/tags.h>
#include <web-xml/parser.h>

namespace Web::Xml::Tests {

test$("parse-empty-document") {
    auto s = Io::SScan(""s);
    auto p = Parser();
    expect$(not p.parse(s, Web::HTML)); // An empty document is invalid
    return Ok();
}

test$("parse-simple-document") {
    auto s = Io::SScan("<html></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    expect$(doc->hasChildren());
    auto first = doc->firstChild();
    expect$(first->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(first.cast<Dom::Element>())->tagName == Html::HTML);
    return Ok();
}

test$("parse-empty-tag") {
    auto s = Io::SScan("<html/>");
    auto p = Parser();
    try$(p.parse(s, Web::HTML));
    return Ok();
}

test$("parse-attr") {
    auto s = Io::SScan("<html lang=\"en\"/>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasAttribute(Html::LANG_ATTR));
    expect$(el->getAttribute(Html::LANG_ATTR) == "en");
    return Ok();
}

} // namespace Web::Xml::Tests

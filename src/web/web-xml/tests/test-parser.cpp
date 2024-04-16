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

test$("parse-open-close-tag") {
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

test$("parse-text") {
    auto s = Io::SScan("<html>text</html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto text = el->firstChild();
    expect$(text->nodeType() == Dom::NodeType::TEXT);
    expect$(try$(text.cast<Dom::Text>())->data == "text");
    return Ok();
}

test$("parse-nested-tags") {
    auto s = Io::SScan("<html><head></head><body></body></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto head = el->firstChild();
    expect$(head->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(head.cast<Dom::Element>())->tagName == Html::HEAD);
    auto body = head->nextSibling();
    expect$(body->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(body.cast<Dom::Element>())->tagName == Html::BODY);
    return Ok();
}

test$("parse-comment") {
    auto s = Io::SScan("<html><!-- comment --></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto comment = el->firstChild();
    expect$(comment->nodeType() == Dom::NodeType::COMMENT);
    expect$(try$(comment.cast<Dom::Comment>())->data == " comment ");
    return Ok();
}

test$("parse-doctype") {
    auto s = Io::SScan("<!DOCTYPE html><html></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto doctype = try$(first.cast<Dom::DocumentType>());
    expect$(doctype->name == "html");
    return Ok();
}

} // namespace Web::Xml::Tests

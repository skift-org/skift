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

test$("parse-text-before-tag") {
    auto s = Io::SScan("<html>text<div/></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto text = el->firstChild();
    expect$(text->nodeType() == Dom::NodeType::TEXT);
    expect$(try$(text.cast<Dom::Text>())->data == "text");
    auto div = text->nextSibling();
    expect$(div->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(div.cast<Dom::Element>())->tagName == Html::DIV);
    return Ok();
}

test$("parse-text-after-tag") {
    auto s = Io::SScan("<html><div/>text</html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto div = el->firstChild();
    expect$(div->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(div.cast<Dom::Element>())->tagName == Html::DIV);
    auto text = div->nextSibling();
    expect$(text->nodeType() == Dom::NodeType::TEXT);
    expect$(try$(text.cast<Dom::Text>())->data == "text");
    return Ok();
}

test$("parse-text-between-tags") {
    auto s = Io::SScan("<html><div/>text<div/></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());
    auto div1 = el->firstChild();
    expect$(div1->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(div1.cast<Dom::Element>())->tagName == Html::DIV);
    auto text = div1->nextSibling();
    expect$(text->nodeType() == Dom::NodeType::TEXT);
    expect$(try$(text.cast<Dom::Text>())->data == "text");
    auto div2 = text->nextSibling();
    expect$(div2->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(div2.cast<Dom::Element>())->tagName == Html::DIV);
    return Ok();
}

test$("parse-text-between-tags-and-before") {
    auto s = Io::SScan("<html>test2<div>text</div></html>");
    auto p = Parser();
    auto doc = try$(p.parse(s, Web::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Dom::Element>());
    expect$(el->hasChildren());

    auto text1 = el->firstChild();
    expect$(text1->nodeType() == Dom::NodeType::TEXT);
    expectEq$(try$(text1.cast<Dom::Text>())->data, "test2");

    auto div = text1->nextSibling();
    expect$(div->nodeType() == Dom::NodeType::ELEMENT);
    expect$(try$(div.cast<Dom::Element>())->tagName == Html::DIV);

    auto text2 = div->firstChild();
    expect$(text2->nodeType() == Dom::NodeType::TEXT);
    expectEq$(try$(text2.cast<Dom::Text>())->data, "text");

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

#include <karm-test/macros.h>
#include <vaev-markup/xml.h>

namespace Vaev::Markup::Tests {

test$("parse-empty-document") {
    auto s = Io::SScan(""s);
    XmlParser p{};
    expect$(not p.parse(s, Vaev::HTML)); // An empty document is invalid
    return Ok();
}

test$("parse-open-close-tag") {
    auto s = Io::SScan("<html></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    expect$(doc->hasChildren());
    auto first = doc->firstChild();
    expect$(first->nodeType() == NodeType::ELEMENT);
    expect$(try$(first.cast<Element>())->tagName == Html::HTML);
    return Ok();
}

test$("parse-empty-tag") {
    auto s = Io::SScan("<html/>");
    XmlParser p{};
    try$(p.parse(s, Vaev::HTML));
    return Ok();
}

test$("parse-attr") {
    auto s = Io::SScan("<html lang=\"en\"/>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasAttribute(Html::LANG_ATTR));
    expect$(el->getAttribute(Html::LANG_ATTR) == "en");
    return Ok();
}

test$("parse-text") {
    auto s = Io::SScan("<html>text</html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto text = el->firstChild();
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(try$(text.cast<Text>())->data == "text");
    return Ok();
}

test$("parse-text-before-tag") {
    auto s = Io::SScan("<html>text<div/></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto text = el->firstChild();
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(try$(text.cast<Text>())->data == "text");
    auto div = text->nextSibling();
    expect$(div->nodeType() == NodeType::ELEMENT);
    expect$(try$(div.cast<Element>())->tagName == Html::DIV);
    return Ok();
}

test$("parse-text-after-tag") {
    auto s = Io::SScan("<html><div/>text</html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto div = el->firstChild();
    expect$(div->nodeType() == NodeType::ELEMENT);
    expect$(try$(div.cast<Element>())->tagName == Html::DIV);
    auto text = div->nextSibling();
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(try$(text.cast<Text>())->data == "text");
    return Ok();
}

test$("parse-text-between-tags") {
    auto s = Io::SScan("<html><div/>text<div/></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto div1 = el->firstChild();
    expect$(div1->nodeType() == NodeType::ELEMENT);
    expect$(try$(div1.cast<Element>())->tagName == Html::DIV);
    auto text = div1->nextSibling();
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(try$(text.cast<Text>())->data == "text");
    auto div2 = text->nextSibling();
    expect$(div2->nodeType() == NodeType::ELEMENT);
    expect$(try$(div2.cast<Element>())->tagName == Html::DIV);
    return Ok();
}

test$("parse-text-between-tags-and-before") {
    auto s = Io::SScan("<html>test2<div>text</div></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());

    auto text1 = el->firstChild();
    expect$(text1->nodeType() == NodeType::TEXT);
    expectEq$(try$(text1.cast<Text>())->data, "test2");

    auto div = text1->nextSibling();
    expect$(div->nodeType() == NodeType::ELEMENT);
    expect$(try$(div.cast<Element>())->tagName == Html::DIV);

    auto text2 = div->firstChild();
    expect$(text2->nodeType() == NodeType::TEXT);
    expectEq$(try$(text2.cast<Text>())->data, "text");

    return Ok();
}

test$("parse-nested-tags") {
    auto s = Io::SScan("<html><head></head><body></body></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto head = el->firstChild();
    expect$(head->nodeType() == NodeType::ELEMENT);
    expect$(try$(head.cast<Element>())->tagName == Html::HEAD);
    auto body = head->nextSibling();
    expect$(body->nodeType() == NodeType::ELEMENT);
    expect$(try$(body.cast<Element>())->tagName == Html::BODY);
    return Ok();
}

test$("parse-comment") {
    auto s = Io::SScan("<html><!-- comment --></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto el = try$(first.cast<Element>());
    expect$(el->hasChildren());
    auto comment = el->firstChild();
    expect$(comment->nodeType() == NodeType::COMMENT);
    expect$(try$(comment.cast<Comment>())->data == " comment ");
    return Ok();
}

test$("parse-doctype") {
    auto s = Io::SScan("<!DOCTYPE html><html></html>");
    XmlParser p{};
    auto doc = try$(p.parse(s, Vaev::HTML));
    auto first = doc->firstChild();
    auto doctype = try$(first.cast<DocumentType>());
    expect$(doctype->name == "html");
    return Ok();
}

} // namespace Vaev::Markup::Tests

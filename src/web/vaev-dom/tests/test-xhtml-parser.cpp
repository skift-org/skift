#include <karm-test/macros.h>
#include <vaev-dom/xml/parser.h>

namespace Vaev::Dom::Tests {

test$("parse-empty-document") {
    Gc::Heap gc;
    auto s = Io::SScan(""s);
    XmlParser p{gc};
    expect$(not p._parseElement(s, Vaev::HTML)); // An empty document is invalid
    return Ok();
}

test$("parse-open-close-tag") {
    Gc::Heap gc;
    XmlParser p{gc};
    auto s = Io::SScan("<html></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->tagName == Html::HTML);

    return Ok();
}

test$("parse-empty-tag") {
    Gc::Heap gc;
    XmlParser p{gc};
    auto s = Io::SScan("<html/>");
    try$(p._parseElement(s, Vaev::HTML));
    return Ok();
}

test$("parse-attr") {
    Gc::Heap gc;
    XmlParser p{gc};
    auto s = Io::SScan("<html lang=\"en\"/>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasAttribute(Html::LANG_ATTR));
    expect$(el->getAttribute(Html::LANG_ATTR) == "en");

    return Ok();
}

test$("parse-text") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html>text</html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto text = el->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "text");

    return Ok();
}

test$("parse-text-before-tag") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html>text<div/></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto text = el->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "text");

    auto div = text->nextSibling()->is<Element>();
    expect$(div->nodeType() == NodeType::ELEMENT);
    expect$(div->tagName == Html::DIV);

    return Ok();
}

test$("parse-text-after-tag") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html><div/>text</html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto div = el->firstChild()->is<Element>();
    expectNe$(div, nullptr);
    expect$(div->tagName == Html::DIV);

    auto text = div->nextSibling()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "text");

    return Ok();
}

test$("parse-text-between-tags") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html><div/>text<div/></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto div1 = el->firstChild()->is<Element>();
    expectNe$(div1, nullptr);
    expect$(div1->nodeType() == NodeType::ELEMENT);
    expect$(div1->tagName == Html::DIV);

    auto text = div1->nextSibling()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(text->data() == "text");

    auto div2 = text->nextSibling()->is<Element>();
    expectNe$(div2, nullptr);
    expect$(div2->nodeType() == NodeType::ELEMENT);
    expect$(div2->tagName == Html::DIV);

    return Ok();
}

test$("parse-text-between-tags-and-before") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html>test2<div>text</div></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));
    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto text1 = el->firstChild()->is<Text>();
    expectNe$(text1, nullptr);
    expect$(text1->nodeType() == NodeType::TEXT);
    expectEq$(text1->data(), "test2");

    auto div = text1->nextSibling()->is<Element>();
    expectNe$(div, nullptr);
    expect$(div->nodeType() == NodeType::ELEMENT);
    expect$(div->tagName == Html::DIV);

    auto text2 = div->firstChild()->is<Text>();
    expectNe$(text2, nullptr);
    expect$(text2->nodeType() == NodeType::TEXT);
    expectEq$(text2->data(), "text");

    return Ok();
}

test$("parse-nested-tags") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html><head></head><body></body></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto head = el->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->nodeType() == NodeType::ELEMENT);
    expect$(head->tagName == Html::HEAD);

    auto body = head->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->nodeType() == NodeType::ELEMENT);
    expect$(body->tagName == Html::BODY);

    return Ok();
}

test$("parse-comment") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<html><!-- comment --></html>");
    auto root = try$(p._parseElement(s, Vaev::HTML));

    auto el = root->is<Element>();
    expectNe$(el, nullptr);
    expect$(el->hasChildren());

    auto comment = el->firstChild()->is<Comment>();
    expectNe$(comment, nullptr);
    expect$(comment->nodeType() == NodeType::COMMENT);
    expect$(comment->data() == " comment ");

    return Ok();
}

test$("parse-doctype") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<!DOCTYPE html><html></html>");

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    try$(p.parse(s, Vaev::HTML, *dom));
    expect$(dom->hasChildren());

    auto doctype = dom->firstChild()->is<DocumentType>();
    expectNe$(doctype, nullptr);
    expect$(doctype->name == "html");

    return Ok();
}

test$("parse-title") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<title>the title</title>");
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    try$(p.parse(s, Vaev::HTML, *dom));
    expect$(dom->title() == "the title");
    return Ok();
}

test$("parse-comment-with-gt-symb") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan(
        "<title>im a title!</title>"
        "<!-- a b <meta> c d -->"
    );
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    try$(p.parse(s, Vaev::HTML, *dom));

    expect$(dom->hasChildren());
    auto title = dom->firstChild()->is<Element>();
    expectNe$(title, nullptr);
    expect$(title->nodeType() == NodeType::ELEMENT);
    expect$(title->tagName == Html::TITLE);
    expect$(title->hasNextSibling());

    auto comment = title->nextSibling()->is<Comment>();
    expectNe$(comment, nullptr);
    expect$(comment->nodeType() == NodeType::COMMENT);
    expect$(comment->data() == " a b <meta> c d ");

    return Ok();
}

test$("parse-xml-decl") {
    Gc::Heap gc;
    XmlParser p{gc};

    auto s = Io::SScan("<?xml version='1.0' encoding='UTF-8'?><html></html>");
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    try$(p.parse(s, Vaev::HTML, *dom));
    expect$(dom->xmlVersion == "1.0");
    expect$(dom->xmlEncoding == "UTF-8");
    expect$(dom->xmlStandalone == "no");
    return Ok();
}

} // namespace Vaev::Dom::Tests

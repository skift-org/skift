#include <karm-test/macros.h>

import Vaev.Engine;
import Karm.Gc;

using namespace Karm;

namespace Vaev::Dom::Tests {

test$("parse-empty-document") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write(""s);
    return Ok();
}

test$("parse-open-close-tag-with-structure") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<html></html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->hasChildren());

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->qualifiedName == Html::HEAD_TAG);

    auto body = head->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    return Ok();
}

test$("parse-empty-tag") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<html/>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    return Ok();
}

test$("parse-attr") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<html lang=\"en\"/>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->hasAttribute(Html::LANG_ATTR));
    expect$(html->getAttribute(Html::LANG_ATTR) == "en");

    return Ok();
}

test$("parse-text") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<html>text</html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);
    expect$(body->hasChildren());

    auto text = body->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "text");

    return Ok();
}

test$("parse-title") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<title>the title</title>");

    expect$(dom->title() == "the title");
    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->countChildren() == 2);

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->qualifiedName == Html::HEAD_TAG);
    expect$(head->hasChildren());

    auto title = head->firstChild()->is<Element>();
    expectNe$(title, nullptr);
    expect$(title->qualifiedName == Html::TITLE_TAG);
    expect$(title->hasChildren());

    auto text = title->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "the title");

    return Ok();
}

test$("parse-comment-with-gt-symb") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write(
        "<title>im a title!</title>"
        "<!-- a b <meta> c d -->"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->qualifiedName == Html::HEAD_TAG);
    expect$(head->hasChildren());

    auto title = head->firstChild()->is<Element>();
    expectNe$(title, nullptr);
    expect$(title->qualifiedName == Html::TITLE_TAG);

    auto comment = title->nextSibling()->is<Comment>();
    expectNe$(comment, nullptr);
    expect$(comment->data() == " a b <meta> c d ");

    return Ok();
}

test$("parse-p-after-comment") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write(
        "<!-- im a comment -->"
        "<p>im a p</p>"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto comment = dom->firstChild()->is<Comment>();
    expectNe$(comment, nullptr);

    auto html = comment->nextSibling()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);
    expect$(body->hasChildren());

    auto p = body->firstChild()->is<Element>();
    expectNe$(p, nullptr);
    expect$(p->qualifiedName == Html::P_TAG);

    auto text = p->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "im a p");

    return Ok();
}

test$("parse-not-nested-p-and-els-inbody") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<div>b</div><p>a<div>b</div><p>a<p>a");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);
    expect$(body->countChildren() == 5);

    return Ok();
}

test$("parse-char-referece-as-text") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<html><body>im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there</body></html>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);
    expect$(body->hasChildren());

    auto text = body->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-as-attribute-value") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<meta value=\"im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there\">");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->qualifiedName == Html::HEAD_TAG);
    expect$(head->hasChildren());

    auto meta = head->firstChild()->is<Element>();
    expectNe$(meta, nullptr);
    expect$(meta->qualifiedName == Html::META_TAG);
    expect$(meta->getAttribute(Html::VALUE_ATTR) == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-spec-example") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write(
        "<html><meta value=\"I'm &notit; I tell you\">"
        "<body><div>I'm &notit; I tell you</div><div>I'm &notin; I tell you</div></body></html>"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);
    expect$(body->hasChildren());

    {
        auto div1 = body->firstChild()->is<Element>();
        expectNe$(div1, nullptr);
        expect$(div1->qualifiedName == Html::DIV_TAG);

        auto text1 = div1->firstChild()->is<Text>();
        expectNe$(text1, nullptr);
        expect$(text1->data() == "I'm ¬it; I tell you");
    }
    {
        auto div2 = body->firstChild()->nextSibling()->is<Element>();
        expectNe$(div2, nullptr);
        expect$(div2->qualifiedName == Html::DIV_TAG);

        auto text2 = div2->firstChild()->is<Text>();
        expectNe$(text2, nullptr);
        expect$(text2->data() == "I'm ∉ I tell you");
    }
    {
        auto head = html->firstChild()->is<Element>();
        expectNe$(head, nullptr);
        expect$(head->qualifiedName == Html::HEAD_TAG);

        auto meta = head->firstChild()->is<Element>();
        expectNe$(meta, nullptr);
        expect$(meta->getAttribute(Html::VALUE_ATTR) == "I'm &notit; I tell you");
    }

    return Ok();
}

test$("parse-input-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<div><input></div>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    auto div = body->firstChild()->is<Element>();
    expectNe$(div, nullptr);
    expect$(div->qualifiedName == Html::DIV_TAG);

    auto input = div->firstChild()->is<Element>();
    expectNe$(input, nullptr);
    expect$(input->qualifiedName == Html::INPUT_TAG);

    return Ok();
}

test$("parse-empty-table-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<table></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->qualifiedName == Html::TABLE_TAG);
    expect$(not table->hasChildren());

    return Ok();
}

test$("parse-table-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<table><thead><tr><th>hi</th></tr></thead></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->qualifiedName == Html::TABLE_TAG);
    expect$(table->countChildren() == 1);

    auto thead = table->firstChild()->is<Element>();
    expectNe$(thead, nullptr);
    expect$(thead->qualifiedName == Html::THEAD_TAG);

    auto headerRow = thead->firstChild()->is<Element>();
    expectNe$(headerRow, nullptr);
    expect$(headerRow->qualifiedName == Html::TR_TAG);

    auto headerCell = headerRow->firstChild()->is<Element>();
    expectNe$(headerCell, nullptr);
    expect$(headerCell->qualifiedName == Html::TH_TAG);

    auto text = headerCell->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "hi");

    return Ok();
}

test$("parse-table-element-create-body-tr-scope") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write("<table><th>hi</th></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->qualifiedName == Html::TABLE_TAG);
    expect$(table->countChildren() == 1);

    auto tbody = table->firstChild()->is<Element>();
    expectNe$(tbody, nullptr);
    expect$(tbody->qualifiedName == Html::TBODY_TAG);

    auto row = tbody->firstChild()->is<Element>();
    expectNe$(row, nullptr);
    expect$(row->qualifiedName == Html::TR_TAG);

    auto header = row->firstChild()->is<Element>();
    expectNe$(header, nullptr);
    expect$(header->qualifiedName == Html::TH_TAG);

    auto text = header->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "hi");

    return Ok();
}

test$("parse-svg-case-fix") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    parser.write(
        "<html><svg viewbox=\"0 0 0 0\"><foreignobject></foreignobject></svg></html>"s
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->qualifiedName == Html::HTML_TAG);
    expect$(html->hasChildren());

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->qualifiedName == Html::HEAD_TAG);

    auto body = head->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->qualifiedName == Html::BODY_TAG);

    auto svg = body->firstChild()->is<Element>();
    expectNe$(svg, nullptr);
    expect$(svg->qualifiedName == Svg::SVG_TAG);
    expect$(svg->countChildren() == 1);
    expect$(svg->hasAttribute(Svg::VIEW_BOX_ATTR));

    auto foreignObject = svg->firstChild()->is<Element>();
    expectNe$(foreignObject, nullptr);
    expect$(foreignObject->qualifiedName == Svg::FOREIGN_OBJECT_TAG);

    return Ok();
}

} // namespace Vaev::Dom::Tests

#include <karm-test/macros.h>
#include <vaev-dom/comment.h>
#include <vaev-dom/html/parser.h>

namespace Vaev::Dom::Tests {

test$("parse-empty-document") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(""s);
    return Ok();
}

test$("parse-open-close-tag-with-structure") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<html></html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->hasChildren());

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->tagName == Html::HEAD);

    auto body = head->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);

    return Ok();
}

test$("parse-empty-tag") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<html/>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    return Ok();
}

test$("parse-attr") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

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
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<html>text</html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto text = body->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "text");

    return Ok();
}

test$("parse-title") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<title>the title</title>");

    expect$(dom->title() == "the title");
    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->countChildren() == 2);

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto title = head->firstChild()->is<Element>();
    expectNe$(title, nullptr);
    expect$(title->tagName == Html::TITLE);
    expect$(title->hasChildren());

    auto text = title->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "the title");

    return Ok();
}

test$("parse-comment-with-gt-symb") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

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
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto title = head->firstChild()->is<Element>();
    expectNe$(title, nullptr);
    expect$(title->tagName == Html::TITLE);

    auto comment = title->nextSibling()->is<Comment>();
    expectNe$(comment, nullptr);
    expect$(comment->data() == " a b <meta> c d ");

    return Ok();
}

test$("parse-p-after-comment") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

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
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto p = body->firstChild()->is<Element>();
    expectNe$(p, nullptr);
    expect$(p->tagName == Html::P);

    auto text = p->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "im a p");

    return Ok();
}

test$("parse-not-nested-p-and-els-inbody") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<div>b</div><p>a<div>b</div><p>a<p>a");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);
    expect$(body->countChildren() == 5);

    return Ok();
}

test$("parse-char-referece-as-text") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<html><body>im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there</body></html>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto text = body->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-as-attribute-value") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<meta value=\"im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there\">");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto head = html->firstChild()->is<Element>();
    expectNe$(head, nullptr);
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto meta = head->firstChild()->is<Element>();
    expectNe$(meta, nullptr);
    expect$(meta->tagName == Html::META);
    expect$(meta->getAttribute(Html::VALUE_ATTR) == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-spec-example") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><meta value=\"I'm &notit; I tell you\">"
        "<body><div>I'm &notit; I tell you</div><div>I'm &notin; I tell you</div></body></html>"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    {
        auto div1 = body->firstChild()->is<Element>();
        expectNe$(div1, nullptr);
        expect$(div1->tagName == Html::DIV);

        auto text1 = div1->firstChild()->is<Text>();
        expectNe$(text1, nullptr);
        expect$(text1->data() == "I'm ¬it; I tell you");
    }
    {
        auto div2 = body->firstChild()->nextSibling()->is<Element>();
        expectNe$(div2, nullptr);
        expect$(div2->tagName == Html::DIV);

        auto text2 = div2->firstChild()->is<Text>();
        expectNe$(text2, nullptr);
        expect$(text2->data() == "I'm ∉ I tell you");
    }
    {
        auto head = html->firstChild()->is<Element>();
        expectNe$(head, nullptr);
        expect$(head->tagName == Html::HEAD);

        auto meta = head->firstChild()->is<Element>();
        expectNe$(meta, nullptr);
        expect$(meta->getAttribute(Html::VALUE_ATTR) == "I'm &notit; I tell you");
    }

    return Ok();
}

test$("parse-input-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<div><input></div>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);

    auto div = body->firstChild()->is<Element>();
    expectNe$(div, nullptr);
    expect$(div->tagName == Html::DIV);

    auto input = div->firstChild()->is<Element>();
    expectNe$(input, nullptr);
    expect$(input->tagName == Html::INPUT);

    return Ok();
}

test$("parse-empty-table-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<table></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->tagName == Html::TABLE);
    expect$(not table->hasChildren());

    return Ok();
}

test$("parse-table-element") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<table><thead><tr><th>hi</th></tr></thead></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->tagName == Html::TABLE);
    expect$(table->countChildren() == 1);

    auto thead = table->firstChild()->is<Element>();
    expectNe$(thead, nullptr);
    expect$(thead->tagName == Html::THEAD);

    auto headerRow = thead->firstChild()->is<Element>();
    expectNe$(headerRow, nullptr);
    expect$(headerRow->tagName == Html::TR);

    auto headerCell = headerRow->firstChild()->is<Element>();
    expectNe$(headerCell, nullptr);
    expect$(headerCell->tagName == Html::TH);

    auto text = headerCell->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "hi");

    return Ok();
}

test$("parse-table-element-create-body-tr-scope") {
    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write("<table><th>hi</th></table>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = dom->firstChild()->is<Element>();
    expectNe$(html, nullptr);
    expect$(html->tagName == Html::HTML);
    expect$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->is<Element>();
    expectNe$(body, nullptr);
    expect$(body->tagName == Html::BODY);

    auto table = body->firstChild()->is<Element>();
    expectNe$(table, nullptr);
    expect$(table->tagName == Html::TABLE);
    expect$(table->countChildren() == 1);

    auto tbody = table->firstChild()->is<Element>();
    expectNe$(tbody, nullptr);
    expect$(tbody->tagName == Html::TBODY);

    auto row = tbody->firstChild()->is<Element>();
    expectNe$(row, nullptr);
    expect$(row->tagName == Html::TR);

    auto header = row->firstChild()->is<Element>();
    expectNe$(header, nullptr);
    expect$(header->tagName == Html::TH);

    auto text = header->firstChild()->is<Text>();
    expectNe$(text, nullptr);
    expect$(text->data() == "hi");

    return Ok();
}

} // namespace Vaev::Dom::Tests

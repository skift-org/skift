export module Vaev.Engine:dom.document;

import Karm.Gc;
import Karm.Ref;
import :dom.node;
import :dom.element;

namespace Vaev::Style {
export struct StyleSheetList;
} // namespace Vaev::Style

namespace Vaev::Dom {

export enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

// https://dom.spec.whatwg.org/#interface-document
export struct Document : Node {
    static constexpr auto TYPE = NodeType::DOCUMENT;

    Ref::Url _url;
    QuirkMode quirkMode{QuirkMode::NO};

    String xmlVersion;
    String xmlEncoding;
    String xmlStandalone = "no"s; // https://www.w3.org/TR/xml/#NT-SDDecl

    Gc::Ptr<Style::StyleSheetList> styleSheets;

    Document(Ref::Url url)
        : _url(url) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    String title() {
        for (auto node : iterDepthFirst()) {
            if (auto element = node->is<Element>())
                if (element->qualifiedName == Html::TITLE_TAG) {
                    return element->textContent();
                }
        }
        return ""s;
    }

    Ref::Url const& url() const {
        return _url;
    }

    Gc::Ptr<Element> documentElement() const {
        for (auto child = firstChild(); child; child = child->nextSibling())
            if (auto el = child->is<Element>())
                return el;
        return nullptr;
    }
};

} // namespace Vaev::Dom

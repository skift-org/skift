#include "document.h"

#include "element.h"

namespace Vaev::Dom {

String Document::title() const {
    String res = ""s;
    iterDepthFirst([&](auto& node) {
        if (auto element = node.template is<Element>()) {
            if (element->tagName == Html::TITLE) {
                res = element->textContent();
                return Iter::BREAK;
            }
        }
        return Iter::CONTINUE;
    });
    return res;
}

Gc::Ptr<Dom::Element> Document::documentElement() const {
    for (auto child = firstChild(); child; child = child->nextSibling())
        if (auto el = child->is<Dom::Element>())
            return el;
    return nullptr;
}

} // namespace Vaev::Dom

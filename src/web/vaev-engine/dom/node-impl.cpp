module Vaev.Engine;

import Karm.Core;
import Karm.Gc;
import Karm.Ref;

import :dom.document;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#ref-for-dom-node-baseuri%E2%91%A0
Ref::Url Node::baseURI() {
    auto document = ownerDocument();
    if (not document)
        panic("node has no owner document");
    return document->url();
}

// https://dom.spec.whatwg.org/#ref-for-dom-node-ownerdocument%E2%91%A0
Gc::Ptr<Document> Node::ownerDocument() {
    for (auto curr = parentNode(); curr; curr = curr->parentNode())
        if (auto doc = curr->is<Document>())
            return *doc;
    return nullptr;
}

void Node::repr(Io::Emit& e) const {
    e("({}", nodeType());
    _repr(e);
    if (hasChildren()) {
        e.indentNewline();
        for (auto child = firstChild(); child; child = child->nextSibling())
            child->repr(e);
        e.deindent();
    }
    e(")\n");
}

} // namespace Vaev::Dom

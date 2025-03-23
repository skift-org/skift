#pragma once

#include <karm-gc/heap.h>

#include "../comment.h"
#include "../document-type.h"
#include "../document.h"
#include "../element.h"

namespace Vaev::Dom {

struct XmlParser {
    Gc::Heap& _heap;

    XmlParser(Gc::Heap& heap)
        : _heap(heap) {
    }

    Res<> parse(Io::SScan& s, Ns ns, Dom::Document& doc);

    Res<> _parseS(Io::SScan& s);

    Res<Str> _parseName(Io::SScan& s);

    Res<> _parseCharData(Io::SScan& s, StringBuilder& sb);

    Res<Gc::Ref<Comment>> _parseComment(Io::SScan& s);

    Res<> _parsePi(Io::SScan& s);

    Res<> _parsePiTarget(Io::SScan& s);

    Res<> _parseCDSect(Io::SScan& s, StringBuilder& sb);

    Res<> _parseVersionInfo(Io::SScan& s);

    Res<Str> _parseXmlDeclAttr(Io::SScan& s, Re::Expr auto name, Re::Expr auto value);

    Res<> _parseXmlDecl(Io::SScan& s, Dom::Document& doc);

    Res<> _parseMisc(Io::SScan& s, Node& parent);

    Res<> _parseProlog(Io::SScan& s, Dom::Document& doc);

    Res<Gc::Ref<DocumentType>> _parseDoctype(Io::SScan& s);

    Res<Gc::Ref<Element>> _parseElement(Io::SScan& s, Ns ns);

    Res<Gc::Ref<Element>> _parseStartTag(Io::SScan& s, Ns ns);

    Res<> _parseAttribute(Io::SScan& s, Ns ns, Element& el);

    Res<String> _parseAttValue(Io::SScan& s);

    Res<> _parseEndTag(Io::SScan& s, Element& el);

    Res<> _parseContentItem(Io::SScan& s, Ns ns, Element& el);

    Res<> _parseContent(Io::SScan& s, Ns ns, Element& el);

    Res<> _parseTextItem(Io::SScan& s, StringBuilder& sb);

    Res<> _parseText(Io::SScan& s, Element& el);

    Res<Gc::Ref<Element>> _parseEmptyElementTag(Io::SScan& s, Ns ns);

    Res<Rune> _parseCharRef(Io::SScan& s);

    Res<Rune> _parseEntityRef(Io::SScan& s);

    Res<Rune> _parseReference(Io::SScan& s);

    Res<> _parseExternalId(Io::SScan& s, DocumentType& docType);
};

} // namespace Vaev::Dom

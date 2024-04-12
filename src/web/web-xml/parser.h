#pragma once

#include <web-dom/comment.h>
#include <web-dom/document.h>
#include <web-dom/element.h>

namespace Web::Xml {

struct Parser {
    StringBuilder _sb;

    void _append(Rune r) {
        _sb.append(r);
    }

    String _flush() {
        if (not _sb.len())
            return ""s;
        return _sb.take();
    }

    Res<Strong<Dom::Document>> parse(Io::SScan &s, Ns ns);

    Res<> _parseS(Io::SScan &s);

    Res<Str> _parseName(Io::SScan &s);

    Res<> _parseCharData(Io::SScan &s);

    Res<Strong<Dom::Comment>> _parseComment(Io::SScan &s);

    Res<> _parsePi(Io::SScan &s);

    Res<> _parsePiTarget(Io::SScan &s);

    Res<> _parseCDSect(Io::SScan &s);

    Res<> _parseVersionInfo(Io::SScan &s);

    Res<> _parseXmlDecl(Io::SScan &s);

    Res<> _parseMisc(Io::SScan &s, Dom::Node &parent);

    Res<> _parseProlog(Io::SScan &s, Dom::Node &parent);

    Res<Strong<Dom::Element>> _parseElement(Io::SScan &s, Ns ns);

    Res<Strong<Dom::Element>> _parseStartTag(Io::SScan &s, Ns ns);

    Res<> _parseAttribute(Io::SScan &s, Ns ns, Dom::Element &el);

    Res<String> _parseAttValue(Io::SScan &s);

    Res<> _parseEndTag(Io::SScan &s, Dom::Element &el);

    Res<> _parseContentItem(Io::SScan &s, Ns ns, Dom::Element &el);

    Res<> _parseContent(Io::SScan &s, Ns ns, Dom::Element &el);

    Res<Strong<Dom::Element>> _parseEmptyElementTag(Io::SScan &s, Ns ns);

    Res<Rune> _parseCharRef(Io::SScan &s);

    Res<Rune> _parseEntityRef(Io::SScan &s);

    Res<Rune> _parseReference(Io::SScan &s);
};

} // namespace Web::Xml

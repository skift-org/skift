module;

#include <karm-core/macros.h>
#include <karm-logger/logger.h>

export module Vaev.Engine:xml.parser;

import Karm.Core;
import Karm.Gc;

import :dom;

using namespace Karm;

namespace Vaev::Xml {

export struct XmlParser {
    Gc::Heap& _heap;

    XmlParser(Gc::Heap& heap)
        : _heap(heap) {
    }

    // 2 MARK: Documents
    // https://www.w3.org/TR/xml/#sec-documents
    Res<> parse(Io::SScan& s, Symbol ns, Dom::Document& doc) {
        // document :: = prolog element Misc *

        try$(_parseProlog(s, doc));
        doc.appendChild(try$(_parseElement(s, ns)));
        while (_parseMisc(s, doc))
            ;

        return Ok();
    }

    // 2.2 MARK: Characters
    // https://www.w3.org/TR/xml/#charsets

    static constexpr auto RE_CHAR =
        '\x09'_re | '\x0A'_re | '\x0D'_re | Re::range(0x20, 0xD7FF) |
        Re::range(0xE000, 0xFFFD) | Re::range(0x10000, 0x10FFFF);

    // 2.3 MARK: Common Syntactic Constructs
    // https://www.w3.org/TR/xml/#sec-common-syn

    static constexpr auto RE_S =
        Re::single(' ', '\t', '\r', '\n');

    static bool _isNameStartChar(Rune r) {
        Array RANGES = {
            Pair<Rune, Rune>{':', ':'},
            Pair<Rune, Rune>{'A', 'Z'},
            Pair<Rune, Rune>{'_', '_'},
            Pair<Rune, Rune>{'a', 'z'},
            Pair<Rune, Rune>{0xC0, 0xD6},
            Pair<Rune, Rune>{0xD8, 0xF6},
            Pair<Rune, Rune>{0xF8, 0x2FF},
            Pair<Rune, Rune>{0x370, 0x37D},
            Pair<Rune, Rune>{0x37F, 0x1FFF},
            Pair<Rune, Rune>{0x200C, 0x200D},
            Pair<Rune, Rune>{0x2070, 0x218F},
            Pair<Rune, Rune>{0x2C00, 0x2FEF},
            Pair<Rune, Rune>{0x3001, 0xD7FF},
            Pair<Rune, Rune>{0xF900, 0xFDCF},
            Pair<Rune, Rune>{0xFDF0, 0xFFFD},
            Pair<Rune, Rune>{0x10000, 0xEFFFF},
        };

        for (auto range : RANGES)
            if (r >= range.v0 and r <= range.v1)
                return true;

        return false;
    }

    static constexpr auto RE_NAME_START_CHAR = Re::ctype(_isNameStartChar);

    static bool _isNameChar(Rune r) {
        Array RANGES = {
            Pair<Rune, Rune>{'-', '-'},
            Pair<Rune, Rune>{'.', '.'},
            Pair<Rune, Rune>{'0', '9'},
            Pair<Rune, Rune>{0xB7, 0xB7},
            Pair<Rune, Rune>{0x0300, 0x036F},
            Pair<Rune, Rune>{0x203F, 0x2040},
        };

        for (auto range : RANGES)
            if (r >= range.v0 and r <= range.v1)
                return true;

        return false;
    }

    static constexpr auto RE_NAME_CHAR = RE_NAME_START_CHAR | Re::ctype(_isNameChar);

    static constexpr auto RE_NAME = RE_NAME_START_CHAR & Re::zeroOrMore(RE_NAME_CHAR);

    Res<> _parseS(Io::SScan& s) {
        // S ::= (#x20 | #x9 | #xD | #xA)+

        s.eat(Re::oneOrMore(RE_S));

        return Ok();
    }

    Res<Str> _parseName(Io::SScan& s) {
        // Name ::= NameStartChar (NameChar)*

        auto name = s.token(RE_NAME);
        if (isEmpty(name))
            return Error::invalidData("expected name");
        return Ok(name);
    }

    // 2.4 MARK: Character Data and Markup
    // https://www.w3.org/TR/xml/#syntax

    static constexpr auto RE_CHARDATA = Re::negate(Re::single('<', '&'));

    Res<> _parseCharData(Io::SScan& s, StringBuilder& sb) {
        // CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)

        bool any = false;

        while (
            s.ahead(RE_CHARDATA) and
            not s.ahead("]]>"_re) and
            not s.ended()
        ) {
            sb.append(s.next());
            any = true;
        }

        if (not any)
            return Error::invalidData("expected character data");

        return Ok();
    }

    // 2.5 MARK: Comments
    // https://www.w3.org/TR/xml/#sec-comments
    static constexpr auto RE_COMMENT_START = "<!--"_re;
    static constexpr auto RE_COMMENT_END = "-->"_re;

    Res<Gc::Ref<Dom::Comment>> _parseComment(Io::SScan& s) {
        // 	Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'

        auto rollback = s.rollbackPoint();

        if (not s.skip(RE_COMMENT_START))
            return Error::invalidData("expected '<!--'");

        StringBuilder sb;
        while (not s.ahead(RE_COMMENT_END) and not s.ended()) {
            auto chrs = s.token(RE_CHAR);
            if (isEmpty(chrs))
                return Error::invalidData("expected character data");
            sb.append(chrs);
        }

        if (not s.skip(RE_COMMENT_END))
            return Error::invalidData("expected '-->'");

        rollback.disarm();
        return Ok(_heap.alloc<Dom::Comment>(sb.take()));
    }

    // 2.6 MARK: Processing Instructions
    // https://www.w3.org/TR/xml/#sec-pi

    static constexpr auto RE_PI_START = "<?"_re;
    static constexpr auto RE_PI_END = "?>"_re;

    Res<> _parsePi(Io::SScan& s) {
        // PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>

        auto rollback = s.rollbackPoint();

        if (not s.skip(RE_PI_START))
            return Error::invalidData("expected '<?'");
        try$(_parsePiTarget(s));

        while (not s.ahead(RE_PI_END) and not s.ended()) {
            auto chrs = s.token(RE_CHAR);
            if (isEmpty(chrs))
                return Error::invalidData("expected character data");
        }

        if (not s.skip(RE_PI_END))
            return Error::invalidData("expected '?>'");

        rollback.disarm();
        return Ok();
    }

    Res<> _parsePiTarget(Io::SScan& s) {
        // PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))

        auto name = try$(_parseName(s));
        if (eqCi(name, "xml"s))
            return Error::invalidData("expected name to not be 'xml'");
        return Ok();
    }

    // 2.7 MARK: CDATA Sections
    // https://www.w3.org/TR/xml/#sec-cdata-sect

    Res<> _parseCDSect(Io::SScan& s, StringBuilder& sb) {
        // CDStart ::= '<![CDATA['
        // CData ::= (Char* - (Char* ']]>' Char*))
        // CDEnd ::= ']]>'

        auto rollback = s.rollbackPoint();

        if (not s.skip("<![CDATA["_re))
            return Error::invalidData("expected '<![CDATA['");

        while (s.match("]]>"_re) == Match::NO and not s.ended())
            sb.append(s.next());

        if (not s.skip("]]>"_re))
            return Error::invalidData("expected ']]>'");

        rollback.disarm();
        return Ok();
    }

    // 2.8 MARK: Prolog and Document Type Declaration
    // https://www.w3.org/TR/xml/#sec-prolog-dtd

    static constexpr auto RE_XML_DECL_START = "<?xml"_re;
    static constexpr auto RE_XML_DECL_VERSION = Re::chain("1."_re, Re::oneOrMore(Re::digit()));
    static constexpr auto RE_XML_DECL_ENCODING = Re::chain(Re::alpha(), Re::zeroOrMore(Re::word() | "."_re | "-"_re));
    static constexpr auto RE_XML_DECL_STANDALONE = "yes"_re | "no"_re;

    Res<Str> _parseXmlDeclAttr(Io::SScan& s, Re::Expr auto name, Re::Expr auto value) {
        auto rollback = s.rollbackPoint();

        try$(_parseS(s));

        if (not s.skip(name))
            return Error::invalidData("expected attribute");

        try$(_parseS(s));

        if (not s.skip('='))
            return Error::invalidData("expected '='");

        try$(_parseS(s));

        auto quote = s.next();
        if (quote != '"' and quote != '\'')
            return Error::invalidData("expected '\"' or '''");

        auto result = s.token(value);
        if (not result)
            return Error::invalidData("expected attribute value");

        if (s.peek() != quote)
            return Error::invalidData("expected closing quote");
        s.next();

        rollback.disarm();
        return Ok(result);
    }

    Res<> _parseXmlDecl(Io::SScan& s, Dom::Document& doc) {
        // XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'

        if (not s.skip(RE_XML_DECL_START))
            return Error::invalidData("expected '<?xml'");

        // versionInfo ::= S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
        auto version = try$(_parseXmlDeclAttr(s, "version"_re, RE_XML_DECL_VERSION));
        logWarnIf(version != "1.0", "Version {} not supported, treating this document as a 1.0 document", version);
        doc.xmlVersion = version;

        // EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
        auto encoding = _parseXmlDeclAttr(s, "encoding"_re, RE_XML_DECL_ENCODING);
        if (encoding.has())
            doc.xmlEncoding = encoding.unwrap();

        // SDDecl ::= S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
        auto standalone = _parseXmlDeclAttr(s, "standalone"_re, RE_XML_DECL_STANDALONE);
        if (standalone.has())
            doc.xmlStandalone = standalone.unwrap();

        try$(_parseS(s));

        if (not s.skip(RE_PI_END))
            return Error::invalidData("expected '?>'");

        return Ok();
    }

    Res<> _parseMisc(Io::SScan& s, Dom::Node& parent) {
        // Misc ::= Comment | PI | S

        auto rollback = s.rollbackPoint();

        if (s.match(RE_COMMENT_START) != Match::NO) {
            auto c = try$(_parseComment(s));
            parent.appendChild(c);
        } else if (s.match(RE_PI_START) != Match::NO)
            try$(_parsePi(s));
        else if (s.match(RE_S) != Match::NO)
            try$(_parseS(s));
        else
            return Error::invalidData("unexpected character");

        rollback.disarm();
        return Ok();
    }

    Res<> _parseProlog(Io::SScan& s, Dom::Document& doc) {
        // prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?
        auto rollback = s.rollbackPoint();

        if (s.match(RE_XML_DECL_START) != Match::NO)
            try$(_parseXmlDecl(s, doc));

        while (_parseMisc(s, doc) and not s.ended())
            ;

        if (auto doctype = _parseDoctype(s)) {
            doc.appendChild(doctype.unwrap());
            while (_parseMisc(s, doc) and not s.ended())
                ;
        }

        rollback.disarm();
        return Ok();
    }

    static constexpr auto RE_DOCTYPE_START = "<!DOCTYPE"_re;

    Res<Gc::Ref<Dom::DocumentType>> _parseDoctype(Io::SScan& s) {
        // doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
        auto rollback = s.rollbackPoint();

        if (not s.skip(RE_DOCTYPE_START))
            return Error::invalidData("expected '<!DOCTYPE'");

        auto docType = _heap.alloc<Dom::DocumentType>();

        try$(_parseS(s));

        docType->name = Symbol::from(try$(_parseName(s)));

        try$(_parseS(s));
        (void)_parseExternalId(s, *docType);

        try$(_parseS(s));
        if (not s.skip('>'))
            return Error::invalidData("expected '>'");

        rollback.disarm();
        return Ok(docType);
    }

    // 2.9 MARK: Standalone Document Declaration
    // https://www.w3.org/TR/xml/#sec-rmd

    // 3 MARK: Logical Structures
    // https://www.w3.org/TR/xml/#sec-logical-struct

    Res<Gc::Ref<Dom::Element>> _parseElement(Io::SScan& s, Symbol ns) {
        // element ::= EmptyElemTag | STag content ETag

        auto rollback = s.rollbackPoint();

        if (auto r = _parseEmptyElementTag(s, ns)) {
            rollback.disarm();
            return r;
        }

        if (auto r = _parseStartTag(s, ns)) {
            auto el = r.unwrap();
            try$(_parseContent(s, el->namespaceUri(), *el));
            try$(_parseEndTag(s, *el));

            rollback.disarm();
            return Ok(el);
        }

        return Error::invalidData("expected element");
    }

    // 3.1 MARK: Start-Tags, End-Tags, and Empty-Element Tags
    // https://www.w3.org/TR/xml/#sec-starttags

    Res<Gc::Ref<Dom::Element>> _parseStartTag(Io::SScan& s, Symbol ns) {
        // STag ::= '<' Name (S Attribute)* S? '>'

        auto rollback = s.rollbackPoint();
        if (not s.skip('<'))
            return Error::invalidData("expected '<'");

        auto name = try$(_parseName(s));
        try$(_parseS(s));

        ns = try$(_parseElementsNamespace(s, ns));

        auto el = _heap.alloc<Dom::Element>(Dom::QualifiedName{ns, Symbol::from(name)});

        while (not s.skip('>') and not s.ended()) {
            try$(_parseAttribute(s, ns, *el));
            try$(_parseS(s));
        }

        rollback.disarm();
        return Ok(el);
    }

    Res<> _parseAttribute(Io::SScan& s, Symbol ns, Dom::Element& el) {
        // Attribute ::= Name Eq AttValue

        auto rollback = s.rollbackPoint();

        auto name = try$(_parseName(s));

        if (not s.skip('='))
            return Error::invalidData("expected '='");

        auto value = try$(_parseAttValue(s));

        // FIXME: the parsing allows rollback so it can be that a warning is emitted when setting an attribute of an element
        // that won't compose the final dom (due to a rollback after a failed parsing)
        // FIXME: this is not a fully compliant XML parser and thus we skip adding xmlns as an attribute to the DOM
        if (name != "xmlns")
            el.setAttribute(Dom::QualifiedName{ns, Symbol::from(name)}, value);

        rollback.disarm();
        return Ok();
    }

    Res<String> _parseAttValue(Io::SScan& s) {
        // AttValue ::= '"' ([^<&"] | Reference)* '"'
        //              |  "'" ([^<&'] | Reference)* "'"

        StringBuilder sb;

        auto rollback = s.rollbackPoint();

        auto quote = s.next();
        if (quote != '"' and quote != '\'')
            return Error::invalidData("expected '\"' or '''");

        while (s.peek() != quote and not s.ended()) {
            if (auto r = _parseReference(s))
                sb.append(r.unwrap());
            else
                sb.append(s.next());
        }

        if (s.peek() != quote)
            return Error::invalidData("expected closing quote");

        s.next();

        rollback.disarm();

        return Ok(sb.take());
    }

    Res<> _parseEndTag(Io::SScan& s, Dom::Element& el) {
        // '</' Name S? '>'

        auto rollback = s.rollbackPoint();

        if (not s.skip("</"_re))
            return Error::invalidData("expected '</'");

        auto name = try$(_parseName(s));
        if (name != el.qualifiedName.name)
            return Error::invalidData("expected end tag name to match start tag name");

        try$(_parseS(s));

        if (not s.skip('>'))
            return Error::invalidData("expected '>'");

        rollback.disarm();
        return Ok();
    }

    Res<> _parseContentItem(Io::SScan& s, Symbol ns, Dom::Element& el) {
        // (element | Reference | CDSect | PI | Comment)

        if (auto r = _parseElement(s, ns)) {
            el.appendChild(r.unwrap());
            return Ok();
        } else if (auto r = _parsePi(s)) {
            logWarn("ignoring processing instruction");
            return Ok();
        } else if (auto r = _parseComment(s)) {
            el.appendChild(r.unwrap());
            return Ok();
        } else {
            return Error::invalidData("expected content item");
        }
    }

    Res<> _parseContent(Io::SScan& s, Symbol ns, Dom::Element& el) {
        // content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*

        try$(_parseText(s, el));
        while (_parseContentItem(s, ns, el))
            try$(_parseText(s, el));

        return Ok();
    }

    Res<> _parseTextItem(Io::SScan& s, StringBuilder& sb) {
        if (_parseCharData(s, sb)) {
            return Ok();
        } else if (_parseCDSect(s, sb)) {
            return Ok();
        } else if (auto r = _parseReference(s)) {
            sb.append(r.unwrap());
            return Ok();
        } else {
            return Error::invalidData("expected text item");
        }
    }

    Res<> _parseText(Io::SScan& s, Dom::Element& el) {
        StringBuilder sb;
        while (_parseTextItem(s, sb))
            ;

        auto te = sb.take();
        if (te)
            el.appendChild(_heap.alloc<Dom::Text>(te));

        return Ok();
    }

    Res<Gc::Ref<Dom::Element>> _parseEmptyElementTag(Io::SScan& s, Symbol ns) {
        // EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'

        auto rollback = s.rollbackPoint();
        if (not s.skip('<'))
            return Error::invalidData("expected '<'");

        auto name = try$(_parseName(s));

        auto el = _heap.alloc<Dom::Element>(Dom::QualifiedName{ns, Symbol::from(name)});
        try$(_parseS(s));
        while (not s.skip("/>"_re) and not s.ended()) {
            try$(_parseAttribute(s, ns, *el));
            try$(_parseS(s));
        }

        rollback.disarm();
        return Ok(el);
    }

    // 4.1 MARK: Character and Entity References
    // https://www.w3.org/TR/xml/#NT-CharRef

    Res<Rune> _parseCharRef(Io::SScan& s) {
        // CharRef ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'

        auto rollback = s.rollbackPoint();

        if (not s.skip("&#"_re))
            return Error::invalidData("expected '&#'");

        Rune r = REPLACEMENT;

        if (s.skip('x')) {
            auto val = Io::atoi(s, {.base = 16});
            if (not val)
                return Error::invalidData("expected hexadecimal number");
            r = val.unwrap();
        } else {
            auto val = Io::atoi(s, {.base = 10});
            if (not val)
                return Error::invalidData("expected decimal number");
            r = val.unwrap();
        }

        if (not s.skip(';'))
            return Error::invalidData("expected ';'");

        rollback.disarm();
        return Ok(r);
    }

    Res<Rune> _parseEntityRef(Io::SScan& s) {
        // EntityRef ::= '&' Name ';'

        auto rollback = s.rollbackPoint();

        if (not s.skip('&'))
            return Error::invalidData("expected '&'");

        auto name = try$(_parseName(s));

        if (not s.skip(';'))
            return Error::invalidData("expected ';'");

        rollback.disarm();
        if (name == "lt")
            return Ok('<');
        else if (name == "gt")
            return Ok('>');
        else if (name == "amp")
            return Ok('&');
        else if (name == "apos")
            return Ok('\'');
        else if (name == "quot")
            return Ok('"');

        // NOTE: (workaround) we don't support XML fully so nbsp can't be loaded from a DOCTYPE as its supposed to
        else if (name == "nbsp")
            return Ok(160);

        rollback.arm();
        return Error::invalidData("unknown entity reference");
    }

    Res<Rune> _parseReference(Io::SScan& s) {
        // Reference ::= EntityRef | CharRef

        if (auto r = _parseCharRef(s))
            return r;
        else if (auto r = _parseEntityRef(s))
            return r;
        else
            return Error::invalidData("expected reference");
    }

    // 4.2 MARK: Entity Declarations
    // https://www.w3.org/TR/xml/#sec-entity-decl

    Res<> _parseExternalId(Io::SScan& s, Dom::DocumentType& docType) {
        // ExternalID ::= 'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral

        auto rollback = s.rollbackPoint();

        if (s.skip("SYSTEM"_re)) {
            try$(_parseS(s));
            // NOSPEC: We are parsing the system literal as att value
            docType.systemId = try$(_parseAttValue(s));
            rollback.disarm();
            return Ok();
        } else if (s.skip("PUBLIC"_re)) {
            // NOSPEC: We are parsing the public and system literals as att values
            try$(_parseS(s));
            docType.publicId = try$(_parseAttValue(s));
            try$(_parseS(s));
            docType.systemId = try$(_parseAttValue(s));
            rollback.disarm();
            return Ok();
        } else {
            return Error::invalidData("expected 'SYSTEM' or 'PUBLIC'");
        }
    }

    // XX MARK: 6.2 Namespace Defaulting
    // https://www.w3.org/TR/xml-names/#dt-defaultNS
    // https://www.w3.org/TR/xml-names/#scoping-defaulting
    // NOTE: Basically same code as attribute parsing, but we need to check for the namespace before parsing the attributes

    Res<Symbol> _parseElementsNamespace(Io::SScan& s, Symbol originalNs) {
        auto rollback = s.rollbackPoint();
        while (not s.skip('>') and not s.ended()) {

            auto name = try$(_parseName(s));

            if (not s.skip('='))
                return Error::invalidData("expected '='");

            auto value = try$(_parseAttValue(s));

            if (name == "xmlns")
                return Ok(Symbol::from(value));

            try$(_parseS(s));
        }
        return Ok(originalNs);
    }
};

} // namespace Vaev::Xml

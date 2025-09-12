export module Vaev.Engine:html.token;

import Karm.Core;

using namespace Karm;

namespace Vaev::Html {

#define FOREACH_TOKEN(TOKEN) \
    TOKEN(NIL)               \
    TOKEN(DOCTYPE)           \
    TOKEN(START_TAG)         \
    TOKEN(END_TAG)           \
    TOKEN(COMMENT)           \
    TOKEN(CHARACTER)         \
    TOKEN(END_OF_FILE)

export struct HtmlToken {
    enum Type {

#define ITER(NAME) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER

            _LEN,
    };

    struct Attr {
        Symbol name = Symbol::EMPTY;
        String value;
    };

    Type type = NIL;
    Symbol name = Symbol::EMPTY;
    Rune rune = '\0';
    String data = ""s;
    String publicIdent = ""s;
    String systemIdent = ""s;
    Vec<Attr> attrs = {};
    bool forceQuirks{false};
    bool selfClosing{false};

    void repr(Io::Emit& e) const {
        e("({}", type);
        if (name)
            e(" name={}", name);
        if (rune)
            e(" rune='{#c}'", rune);
        if (data)
            e(" data={#}", data);
        if (publicIdent)
            e(" publicIdent={#}", publicIdent);
        if (systemIdent)
            e(" systemIdent={#}", systemIdent);
        if (attrs.len() > 0) {
            e.indentNewline();
            for (auto& attr : attrs)
                e("({} {#})", attr.name, attr.value);
            e.deindent();
        }
        if (forceQuirks)
            e(" forceQuirks");
        if (selfClosing)
            e(" selfClosing");
        e(")");
    }

    bool hasAttribute(Str name) const {
        for (auto& attr : attrs) {
            if (attr.name == name)
                return true;
        }
        return false;
    }
};

export struct HtmlSink {
    virtual ~HtmlSink() = default;
    virtual void accept(HtmlToken& token) = 0;
};

} // namespace Vaev::Html

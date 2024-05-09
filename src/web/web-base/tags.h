#pragma once

#include <karm-base/string.h>
#include <karm-io/fmt.h>

namespace Web {

namespace Html {

enum struct TagId : u16;

enum struct AttrId : u16;

Str _tagName(TagId id);

Str _attrName(AttrId id);

Opt<TagId> _tagId(Str name);

Opt<AttrId> _attrId(Str name);

} // namespace Html

namespace Svg {

enum struct TagId : u16;

enum struct AttrId : u16;

Str _tagName(TagId id);

Str _attrName(AttrId id);

Opt<TagId> _tagId(Str name);

Opt<AttrId> _attrId(Str name);

} // namespace Svg

namespace MathMl {

enum struct TagId : u16;

enum struct AttrId : u16;

Str _tagName(TagId id);

Str _attrName(AttrId id);

Opt<TagId> _tagId(Str name);

Opt<AttrId> _attrId(Str name);

} // namespace MathMl

struct Ns {
    enum struct _Id : u8 {
#define NAMESPACE(IDENT, _) IDENT,
#include "defs/namespaces.inc"
#undef NAMESPACE
    };

    _Id _id;

    static Opt<Ns> fromUrl(Str url) {
#define NAMESPACE(IDENT, URL) \
    if (url == URL)           \
        return _Id::IDENT;
#include "defs/namespaces.inc"
#undef NAMESPACE
        return NONE;
    }

    constexpr Ns(_Id id)
        : _id(id) {}

    constexpr Str url() const {
        switch (_id) {
#define NAMESPACE(IDENT, URL) \
    case _Id::IDENT:          \
        return URL;
#include "defs/namespaces.inc"
#undef NAMESPACE
        }
    }

    constexpr Str name() const {
        switch (_id) {
#define NAMESPACE(IDENT, _) \
    case _Id::IDENT:        \
        return #IDENT;      \
        break;
#include "defs/namespaces.inc"
#undef NAMESPACE
        }
    }

    constexpr bool operator==(Ns const &other) const {
        return _id == other._id;
    }
};

#define NAMESPACE(IDENT, URL) \
    inline constexpr Ns IDENT = Ns{Ns::_Id::IDENT};
#include "defs/namespaces.inc"
#undef NAMESPACE

struct TagName {
    u16 id;
    Ns ns;

    static Opt<TagName> tryMake(Str name, Ns ns) {
        if (ns == HTML) {
            auto id = Html::_tagId(name);
            if (id) {
                return TagName{*id};
            }
        } else if (ns == SVG) {
            auto id = Svg::_tagId(name);
            if (id) {
                return TagName{*id};
            }
        } else if (ns == MathML) {
            auto id = MathMl::_tagId(name);
            if (id) {
                return TagName{*id};
            }
        }

        return NONE;
    }

    static TagName make(Str name, Ns ns) {
        auto result = tryMake(name, ns);
        if (result) {
            return *result;
        }
        return TagName{0, ns};
    }

    constexpr TagName(u16 id, Ns ns)
        : id(id), ns(ns) {}

    constexpr TagName(Html::TagId id)
        : id(static_cast<u16>(id)), ns(HTML) {}

    constexpr TagName(Svg::TagId id)
        : id(static_cast<u16>(id)), ns(SVG) {}

    constexpr TagName(MathMl::TagId id)
        : id(static_cast<u16>(id)), ns(MathML) {}

    Str name() const {
        if (ns == HTML) {
            return Html::_tagName(static_cast<Html::TagId>(id));
        } else if (ns == SVG) {
            return Svg::_tagName(static_cast<Svg::TagId>(id));
        } else if (ns == MathML) {
            return MathMl::_tagName(static_cast<MathMl::TagId>(id));
        } else {
            return "unknown";
        }
    }

    constexpr bool operator==(TagName const &other) const = default;
};

struct AttrName {
    u16 id;
    Ns ns;

    static Opt<AttrName> tryMake(Str name, Ns ns) {
        if (ns == HTML) {
            auto id = Html::_attrId(name);
            if (id) {
                return AttrName{*id};
            }
        } else if (ns == SVG) {
            auto id = Svg::_attrId(name);
            if (id) {
                return AttrName{*id};
            }
        } else if (ns == MathML) {
            auto id = MathMl::_attrId(name);
            if (id) {
                return AttrName{*id};
            }
        }

        return NONE;
    }

    static AttrName make(Str name, Ns ns) {
        auto result = tryMake(name, ns);
        if (result) {
            return *result;
        }
        return AttrName{0, ns};
    }

    constexpr AttrName(u16 id, Ns ns)
        : id(id), ns(ns) {}

    constexpr AttrName(Html::AttrId id)
        : id(static_cast<u16>(id)), ns(HTML) {}

    constexpr AttrName(Svg::AttrId id)
        : id(static_cast<u16>(id)), ns(SVG) {}

    constexpr AttrName(MathMl::AttrId id)
        : id(static_cast<u16>(id)), ns(MathML) {}

    Str name() const {
        if (ns == HTML) {
            return Html::_attrName(static_cast<Html::AttrId>(id));
        } else if (ns == SVG) {
            return Svg::_attrName(static_cast<Svg::AttrId>(id));
        } else if (ns == MathML) {
            return MathMl::_attrName(static_cast<MathMl::AttrId>(id));
        } else {
            return "__unknown__";
        }
    }

    constexpr bool operator==(AttrName const &other) const = default;
};

} // namespace Web

template <>
struct Karm::Io::Formatter<Web::Ns> {
    Res<usize> format(Io::TextWriter &writer, Web::Ns const &val) {
        return writer.writeStr(val.name());
    }
};

template <>
struct Karm::Io::Formatter<Web::TagName> {
    Res<usize> format(Io::TextWriter &writer, Web::TagName const &val) {
        return Io::format(writer, "{}:{}", val.ns, val.name());
    }
};

template <>
struct Karm::Io::Formatter<Web::AttrName> {
    Res<usize> format(Io::TextWriter &writer, Web::AttrName const &val) {
        return Io::format(writer, "{}:{}", val.ns, val.name());
    }
};

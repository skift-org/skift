#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <web-dom/element.h>
#include <web-html/tags.h>

namespace Web::Select {

// 17. Calculating a selector’s specificity
// https://www.w3.org/TR/selectors-4/#specificity-rules
struct Spec {
    bool match;
    // a: The number of ID selectors in the selector.
    // b: The number of class selectors, attributes selectors, and pseudo-classes in the selector.
    // c: The number of type selectors and pseudo-elements in the selector.
    isize a, b, c;

    static Spec const NOMATCH;
    static Spec const ZERO, A, B, C;

    Spec(None)
        : match(false), a(0), b(0), c(0) {}

    Spec(isize a, isize b, isize c)
        : match(true), a(a), b(b), c(c) {}

    Spec operator+(Spec const &other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator and(Spec const &other) const {
        if (!match or !other.match)
            return Spec::NOMATCH;
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator or(Spec const &other) const {
        if (*this > other)
            return *this;
        return other;
    }

    Spec operator not() const {
        return {
            a,
            b,
            c
        };
    }

    explicit operator bool() const {
        return match;
    }

    bool operator==(Spec const &other) const = default;
    auto operator<=>(Spec const &other) const = default;
};

Spec const Spec::NOMATCH = NONE;
Spec const Spec::ZERO = {0, 0, 0};
Spec const Spec::A = {1, 0, 0};
Spec const Spec::B = {0, 1, 0};
Spec const Spec::C = {0, 0, 1};

struct Selector;

struct Combinator {
    enum struct _Type {
        LIST,
        DESCENDANT,
        CHILD,
        ADJACENT,
        SUBSEQUENT,
        COLUMN,
        IS,
        NOT,
        WHERE,
    };

    using enum _Type;
    _Type type;
    Vec<Selector> inners;
};

struct TypeSelector {
    TagName type;
};

struct UniversalSelector {
};

struct IdSelector {
    String id;
};

struct ClassSelector {
    String class_;
};

struct AnB {
    isize a, b;
};

enum struct Dir {
    LTR,
    RTL,
};

struct PseudoClass {
    enum struct _Type {
#define PSEUDO_CLASS(ID, ...) ID,
#include "defs/pseudo-class.def"
#undef PSEUDO_CLASS
    };

    using enum _Type;

    _Type type;
    Union<None, String, AnB, Dir> extra = NONE;
    PseudoClass(_Type type)
        : type(type) {}
};

struct AttributeSelector {
    enum Case {
        SENSITIVE,
        INSENSITIVE,
    };

    enum Match {
        PRESENT, //< [attr]

        EXACT,      //< [attr="value"]
        CONTAINS,   //< [attr~="value"]
        HYPHENATED, //< [attr|="value"]

        STR_START_WITH, //< [attr^="value"]
        STR_END_WITH,   //< [attr$="value"]
        STR_CONTAIN,    //< [attr*="value"]
    };

    String name;
    String value;
};

using _Selector = Union<
    Combinator,
    TypeSelector,
    IdSelector,
    ClassSelector,
    PseudoClass,
    AttributeSelector>;

struct Selector : public _Selector {
    using _Selector::_Selector;
};

Spec match(Selector const &sel, Dom::Element &el);

} // namespace Web::Select

template <>
struct Karm::Io::Formatter<Web::Select::Spec> {
    Res<usize> format(Io::TextWriter &writer, Web::Select::Spec const &val) {
        if (!val.match)
            return Io::format(writer, "NOMATCH");
        return Io::format(writer, "{}-{}-{}", val.a, val.b, val.c);
    }
};

template <>
struct Karm::Io::Formatter<Web::Select::AnB> {
    Res<usize> format(Io::TextWriter &writer, Web::Select::AnB const &val) {
        return Io::format(writer, "{}n{}{}", val.a, val.b < 0 ? "-" : "+", val.b);
    }
};

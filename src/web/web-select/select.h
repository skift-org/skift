#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <web-dom/element.h>
#include <web-html/tags.h>

namespace Web::Select {

// 17. Calculating a selector’s specificity
// https://www.w3.org/TR/selectors-4/#specificity-rules
struct Spec {
    // a: The number of ID selectors in the selector.
    // b: The number of class selectors, attributes selectors, and pseudo-classes in the selector.
    // c: The number of type selectors and pseudo-elements in the selector.
    isize a, b, c;

    static Spec const NOMATCH;
    static Spec const ZERO, A, B, C;

    Spec(isize a, isize b, isize c)
        : a(a), b(b), c(c) {}

    Spec operator+(Spec const &other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator and(Spec const &other) const {
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

    bool operator==(Spec const &other) const = default;
    auto operator<=>(Spec const &other) const = default;
};

inline Spec const Spec::ZERO = {0, 0, 0};
inline Spec const Spec::A = {1, 0, 0};
inline Spec const Spec::B = {0, 1, 0};
inline Spec const Spec::C = {0, 0, 1};

struct Selector;

struct UniversalSelector {
    int __dummy__ = 0; // FIXME: Reflectable doesn't like that we don't have filds
};

static constexpr UniversalSelector UNIVERSAL = {};

struct Infix {
    enum struct Type {
        DESCENDANT,
        CHILD,
        ADJACENT,
        SUBSEQUENT,
        COLUMN,
    };

    using enum Type;
    Type type;
    Box<Selector> lhs;
    Box<Selector> rhs;
};

struct Nfix {
    enum struct Type {
        AND, // ''
        OR,  // :is(), ', '
        NOT,
        WHERE,
    };

    using enum Type;

    Type type;
    Vec<Selector> inners;
};

struct TypeSelector {
    TagName type;
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
    enum struct Type {
#define PSEUDO_CLASS(ID, ...) ID,
#include "defs/pseudo-class.def"
#undef PSEUDO_CLASS
    };

    using enum Type;
    using Extra = Union<None, String, AnB, Dir>;

    Type type;
    Extra extra = NONE;
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
    Nfix,
    Infix,
    TypeSelector,
    UniversalSelector,
    IdSelector,
    ClassSelector,
    PseudoClass,
    AttributeSelector>;

struct Selector : public _Selector {
    using _Selector::_Selector;

    static Selector universal() {
        return UNIVERSAL;
    }

    static Selector and_(Vec<Selector> selectors) {
        return Nfix{
            Nfix::AND,
            std::move(selectors),
        };
    }

    static Selector or_(Vec<Selector> selectors) {
        return Nfix{
            Nfix::OR,
            std::move(selectors),
        };
    }

    static Selector not_(Selector selector) {
        return Nfix{
            Nfix::NOT,
            {std::move(selector)},
        };
    }

    static Selector where(Selector selector) {
        return Nfix{
            Nfix::WHERE,
            {std::move(selector)},
        };
    }

    static Selector descendant(Selector lhs, Selector rhs) {
        return Infix{
            Infix::DESCENDANT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector child(Selector lhs, Selector rhs) {
        return Infix{
            Infix::CHILD,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector adjacent(Selector lhs, Selector rhs) {
        return Infix{
            Infix::ADJACENT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector subsequent(Selector lhs, Selector rhs) {
        return Infix{
            Infix::SUBSEQUENT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector column(Selector lhs, Selector rhs) {
        return Infix{
            Infix::COLUMN,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }
};

Spec spec(Selector const &sel);

bool match(Selector const &sel, Dom::Element &el);

} // namespace Web::Select

template <>
struct Karm::Io::Formatter<Web::Select::Spec> {
    Res<usize> format(Io::TextWriter &writer, Web::Select::Spec const &val) {
        return Io::format(writer, "{}-{}-{}", val.a, val.b, val.c);
    }
};

template <>
struct Karm::Io::Formatter<Web::Select::AnB> {
    Res<usize> format(Io::TextWriter &writer, Web::Select::AnB const &val) {
        return Io::format(writer, "{}n{}{}", val.a, val.b < 0 ? "-"s : "+"s, val.b);
    }
};

Reflectable$(Web::Select::UniversalSelector, __dummy__);

Reflectable$(Web::Select::Infix, type, lhs, rhs);

Reflectable$(Web::Select::Nfix, type, inners);

Reflectable$(Web::Select::TypeSelector, type);

Reflectable$(Web::Select::IdSelector, id);

Reflectable$(Web::Select::ClassSelector, class_);

Reflectable$(Web::Select::PseudoClass, type, extra);

Reflectable$(Web::Select::AttributeSelector, name, value);

template <>
struct Karm::Io::Formatter<Web::Select::Selector> {
    Res<usize> format(Io::TextWriter &writer, Web::Select::Selector const &val) {
        return val.visit([&](auto const &v) -> Res<usize> {
            return Io::format(writer, "{}", v);
        });
    }
};

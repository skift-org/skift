#pragma once

#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <vaev-css/parser.h>
#include <vaev-markup/dom.h>
#include <vaev-markup/tags.h>

namespace Vaev::Style {

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

    Spec operator+(Spec const& other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator and(Spec const& other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator or(Spec const& other) const {
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

    bool operator==(Spec const& other) const = default;
    auto operator<=>(Spec const& other) const = default;

    void repr(Io::Emit& e) const {
        e("{}-{}-{}", a, b, c);
    }
};

inline Spec const Spec::ZERO = {0, 0, 0};
inline Spec const Spec::A = {1, 0, 0};
inline Spec const Spec::B = {0, 1, 0};
inline Spec const Spec::C = {0, 0, 1};

Spec const INLINE_SPEC = Spec::ZERO;

struct Selector;

struct UniversalSelector {
    void repr(Io::Emit& e) const {
        e("*");
    }

    bool operator==(UniversalSelector const&) const = default;
};

static constexpr UniversalSelector UNIVERSAL = {};

struct EmptySelector {
    void repr(Io::Emit& e) const {
        e("EMPTY");
    }

    bool operator==(EmptySelector const&) const = default;
};

static constexpr EmptySelector EMPTY = {};

struct Infix {
    enum struct Type {
        DESCENDANT, // ' '
        CHILD,      // >
        ADJACENT,   // +
        SUBSEQUENT, // ~
        COLUMN,     // ||
        _LEN,
    };

    using enum Type;
    Type type;
    Box<Selector> lhs;
    Box<Selector> rhs;

    void repr(Io::Emit& e) const {
        e("({} {} {})", *lhs, type, *rhs);
    }

    bool operator==(Infix const&) const;
};

struct Nfix {
    // NOTE: is(), not() and where() are coded as Nfixes instead of Pseudo
    enum struct Type {
        AND,   // ''
        OR,    // :is(), ', '
        NOT,   // :not()
        WHERE, // :where()
        _LEN,
    };

    using enum Type;

    Type type;
    Vec<Selector> inners;

    void repr(Io::Emit& e) const {
        e("({} {})", type, inners);
    }

    bool operator==(Nfix const&) const;
};

struct TypeSelector {
    TagName type;

    void repr(Io::Emit& e) const {
        e("{}", type);
    }

    bool operator==(TypeSelector const&) const = default;
};

struct IdSelector {
    String id;

    void repr(Io::Emit& e) const {
        e("#{}", id);
    }

    bool operator==(IdSelector const&) const = default;
};

struct ClassSelector {
    String class_;

    void repr(Io::Emit& e) const {
        e(".{}", class_);
    }

    bool operator==(ClassSelector const&) const = default;
};

struct AnB {
    isize a, b;

    void repr(Io::Emit& e) const {
        e("{}n{}{}", a, b < 0 ? "-"s : "+"s, b);
    }

    bool operator==(AnB const&) const = default;
};

enum struct Dir {
    LTR,
    RTL,
};

struct Pseudo {
    enum struct Type {
#define PSEUDO(ID, ...) ID,
#include "defs/pseudo.inc"
#undef PSEUDO

        _LEN,
    };

    static Opt<Type> _Type(Str name) {
#define PSEUDO(IDENT, NAME) \
    if (name == NAME)       \
        return Type::IDENT;
#include "defs/pseudo.inc"
#undef PSEUDO

        return NONE;
    }

    static Pseudo make(Str name) {
        auto id = _Type(name);
        // logDebug("make type {} {}", name, id);
        if (id) {
            auto result = Type{*id};
            return result;
        }
        return Type{0};
    }

    using enum Type;
    using Extra = Union<None, String, AnB, Dir>;

    Type type;
    Extra extra = NONE;

    Pseudo() = default;

    Pseudo(Type type, Extra extra = NONE)
        : type(type), extra(extra) {}

    bool operator==(Pseudo const&) const = default;

    void repr(Io::Emit& e) const {
        e("{}", type);
    }
};

struct AttributeSelector {
    enum Case {
        SENSITIVE,
        INSENSITIVE,

        _LEN0,
    };

    enum Match {
        PRESENT, //< [attr]

        EXACT,      //< [attr="value"]
        CONTAINS,   //< [attr~="value"]
        HYPHENATED, //< [attr|="value"]

        STR_START_WITH, //< [attr^="value"]
        STR_END_WITH,   //< [attr$="value"]
        STR_CONTAIN,    //< [attr*="value"]

        _LEN1,
    };

    String name;
    Case case_;
    Match match;
    String value;

    void repr(Io::Emit& e) const {
        e("[{} {} {} {}]", name, case_, match, value);
    }

    bool operator==(AttributeSelector const&) const = default;
};

using _Selector = Union<
    Nfix,
    Infix,
    TypeSelector,
    UniversalSelector,
    EmptySelector,
    IdSelector,
    ClassSelector,
    Pseudo,
    AttributeSelector>;

struct Selector : public _Selector {
    using _Selector::_Selector;

    Selector() : _Selector{UniversalSelector{}} {}

    static Selector universal() {
        return UNIVERSAL;
    }

    static Selector empty() {
        return EMPTY;
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

    void repr(Io::Emit& e) const {
        visit([&](auto const& v) {
            e("{}", v);
        });
    }

    bool operator==(Selector const&) const = default;

    static Res<Selector> parse(Cursor<Css::Sst>& c);

    static Res<Selector> parse(Io::SScan& s);

    static Res<Selector> parse(Str input);
};

inline bool Infix::operator==(Infix const&) const = default;

inline bool Nfix::operator==(Nfix const&) const = default;

Spec spec(Selector const& sel);

} // namespace Vaev::Style

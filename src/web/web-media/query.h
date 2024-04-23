#pragma once

#include <karm-base/box.h>

#include "features.h"

namespace Web::Media {

struct Query {
    struct _Infix {
        enum struct Type {
            AND,
            OR,
        };

        Type type;
        Box<Query> lhs;
        Box<Query> rhs;

        bool match(Media const &media) const {
            switch (type) {
            case Type::AND:
                return lhs->match(media) and rhs->match(media);
            case Type::OR:
                return lhs->match(media) or rhs->match(media);
            default:
                return false;
            }
        }
    };

    struct _Prefix {
        enum struct Type {
            NOT,
            ONLY,
        };

        Type type;
        Box<Query> query;

        bool match(Media const &media) const {
            switch (type) {
            case Type::NOT:
                return not query->match(media);
            case Type::ONLY:
                return query->match(media);
            default:
                return false;
            }
        }
    };

    using _Store = Union<
        _Infix,
        _Prefix,
        Feature>;

    using enum _Infix::Type;
    using enum _Prefix::Type;

    _Store _store;

    Query(Feature feature) : _store(feature) {}

    Query(Meta::Convertible<Feature> auto &&feature)
        : _store(Feature{
              std::forward<decltype(feature)>(feature),
          }) {}

    Query(_Prefix::Type type, Query query)
        : _store(_Prefix{
              type,
              makeBox<Query>(query),
          }) {}

    Query(_Infix::Type type, Query lhs, Query rhs)
        : _store(_Infix{
              type,
              makeBox<Query>(lhs),
              makeBox<Query>(rhs),
          }) {}

    static Query negate(Query query) {
        return Query{NOT, query};
    }

    static Query only(Query query) {
        return Query{ONLY, query};
    }

    static Query combineAnd(Query lhs, Query rhs) {
        return Query{AND, lhs, rhs};
    }

    static Query combineOr(Query lhs, Query rhs) {
        return Query{OR, lhs, rhs};
    }

    bool match(Media const &media) const {
        return _store.visit([&](auto const &value) {
            return value.match(media);
        });
    }
};

} // namespace Web::Media

export module Karm.Core:base.iter;

import :base.opt;
import :base.clamp;

namespace Karm {

export template <typename Next>
struct Iter {
    Next next;
    using Item = decltype(next());
    using Value = Meta::RemoveConstVolatileRef<decltype(*next())>;

    constexpr Iter(Next next) : next(next) {}

    constexpr auto map(auto f) {
        auto n = [=, *this] mutable -> Opt<decltype(f(*Meta::declval<Item>()))> {
            auto v = next();

            if (not v) {
                return NONE;
            }

            return f(*v);
        };

        return Iter<decltype(n)>{n};
    }

    constexpr auto mapi(auto f) {
        auto n = [=, *this, index = 0uz] mutable -> Opt<decltype(f(*Meta::declval<Item>(), 1uz))> {
            auto v = next();
            index++;

            if (not v) {
                return NONE;
            }

            return f(*v, index - 1);
        };

        return Iter<decltype(n)>{n};
    }

    constexpr auto filter(auto f) {
        auto n = [=, *this] mutable -> Item {
            auto v = next();
            if (not v) {
                return NONE;
            }

            while (not f(*v)) {
                v = next();
                if (not v) {
                    return NONE;
                }
            }

            return v;
        };

        return Iter<decltype(n)>{n};
    }

    constexpr auto reduce(auto init, auto f) {
        auto result = init;
        forEach([&](auto v) {
            result = f(result, v);
        });
        return result;
    }

    constexpr auto forEach(auto f) {
        for (auto item = next(); item; item = next()) {
            f(*item);
        }
    }

    constexpr auto find(auto f) {
        for (auto item = next(); item; item = next()) {
            if (f(*item)) {
                return *item;
            }
        }

        return NONE;
    }

    constexpr usize len() {
        usize result = 0;
        forEach([&](auto) {
            result++;
        });
        return result;
    }

    constexpr auto sum() {
        return reduce(Value{}, [](auto& a, auto& b) {
            return a + b;
        });
    }

    constexpr bool any() {
        return next() != NONE;
    }

    constexpr auto first() {
        return next();
    }

    constexpr auto first(auto f) {
        for (auto item = next(); item; item = next()) {
            if (f(*item)) {
                return item;
            }
        }

        return NONE;
    }

    constexpr auto last() {
        Item result = NONE;
        for (auto item = next(); item; item = next()) {
            result = item;
        }
        return result;
    }

    constexpr auto last(auto f) {
        Item result = NONE;
        for (auto item = next(); item; item = next()) {
            if (f(*item)) {
                result = item;
            }
        }
        return result;
    }

    constexpr auto skip(usize n) {
        for (usize i = 0; i < n; i++) {
            (void)next();
        }
        return *this;
    }

    constexpr auto cycle(usize n) {
        return Iter{[start = *this, curr = *this, i = 0, n] mutable {
            auto v = curr.next();

            if (not v and i < n) {
                curr = start;
                i++;
                v = curr.next();
            }

            return v;
        }};
    }

    constexpr auto take(usize n) {
        return Iter{[=, *this] mutable {
            if (n == 0) {
                return NONE;
            }

            auto v = next();
            n--;
            return v;
        }};
    }

    constexpr auto prepend(auto v) {
        return Iter{
            [=, *this, first = true] mutable -> Item {
                if (first)
                    return Item{v};

                return next();
            }
        };
    }

    constexpr auto append(auto v) {
        return Iter{
            [=, *this, last = true] mutable -> Item {
                auto result = next();

                if (not result and last) {
                    last = false;
                    return Item{v};
                }

                return result;
            }
        };
    }

    constexpr auto at(usize n) {
        return skip(n).first();
    }

    constexpr bool any(auto f) {
        return find(f) != NONE;
    }

    constexpr bool all(auto f) {
        for (auto item = next(); item; item = next()) {
            if (not f(*item)) {
                return false;
            }
        }

        return true;
    }

    constexpr auto min() {
        Item result = NONE;

        for (auto item = next(); item; item = next()) {
            if (not result or (*item < *result)) {
                result = item;
            }
        }

        return result;
    }

    constexpr auto max() {
        Item result = NONE;

        for (auto item = next(); item; item = next()) {
            if (not result or (*item > *result)) {
                result = item;
            }
        }

        return result;
    }

    constexpr auto avg() -> Item {
        Item result = NONE;
        usize count = 0;

        for (auto item = next(); item; item = next()) {
            if (not result) {
                result = item;
            } else {
                result = *result + *item;
            }
            count++;
        }

        if (count == 0) {
            return NONE;
        }

        return *result / count;
    }

    struct It {
        Item curr;
        Iter iter;

        constexpr auto& operator*() {
            return *curr;
        }

        constexpr auto operator++() {
            curr = iter.next();
            return *this;
        }

        constexpr bool operator!=(None) {
            return curr != NONE;
        }
    };

    constexpr It begin() {
        return It{next(), *this};
    }

    constexpr None end() {
        return NONE;
    }

    constexpr auto collect() {
        forEach([&](auto const&...) {
        });
    }

    template <typename C>
    constexpr auto collect(C& c) {
        forEach([&](auto v) {
            c.pushBack(v);
        });
    }

    template <typename C>
    constexpr auto collect(C& c, auto f) {
        forEach([&](auto v) {
            c.pushBack(f(v));
        });
    }

    template <typename C>
    constexpr auto collect() {
        C c;
        collect(c);
        return c;
    }

    template <typename C>
    constexpr auto collect(auto f) {
        C c;
        collect(c, f);
        return c;
    }
};

export template <typename T>
constexpr auto single(T value) {
    return Iter<None>{[value, end = false] mutable {
        if (end) {
            return NONE;
        }

        end = true;
        return value;
    }};
}

export template <typename T>
constexpr auto repeat(T value, usize count) {
    return Iter{[value, count] mutable -> Opt<T> {
        if (count == 0) {
            return NONE;
        }

        count--;
        return value;
    }};
}

export template <typename T>
constexpr auto range(T end) {
    return Iter{[value = static_cast<T>(0), end] mutable -> Opt<T> {
        if (value >= end)
            return NONE;
        return value++;
    }};
}

export template <typename T>
constexpr auto range(T start, T end) {
    return Iter{[value = start, start, end] mutable -> Opt<T> {
        if (value >= end) {
            return NONE;
        }

        auto result = value;
        if (start < end) {
            value++;
        } else {
            value++;
        }
        return result;
    }};
}

export template <typename T>
constexpr auto range(T start, T end, T step) {
    return Iter{[value = start, start, end, step] mutable -> Opt<T> {
        if (value >= end) {
            return NONE;
        }

        auto result = value;
        if (start < end) {
            value += step;
        } else {
            value -= step;
        }
        return result;
    }};
}

export template <typename T>
concept AtLen = requires(T t) {
    t.len();
    t.at(0);
};

export constexpr auto iter(AtLen auto& o) {
    return range(o.len())
        .map([&](auto i) mutable {
            return o.at(i);
        });
}

// MARK: Iter2 ----------------------------------------------------------------

// NOTE: New iterator interface, that will replace the old one.
// This is a work in progress and not all features are implemented yet.

export template <typename T>
concept Iter2Item = requires(T t) {
    *t;
    t == NONE;
    t != NONE;
};

export template <typename T>
concept Iter2 = requires(T t) {
    { t.next() } -> Iter2Item;
};

// MARK: Generator -------------------------------------------------------------

export template <typename T>
struct [[nodiscard, clang::coro_return_type, clang::coro_lifetimebound]] Generator {
    struct promise_type;

    struct promise_type {
        Opt<T> _value = NONE;

        Generator get_return_object() {
            return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        std::suspend_always initial_suspend() { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        void unhandled_exception() {
            panic("unhandled exception in coroutine");
        }

        template <Meta::Convertible<T> From>
        std::suspend_always yield_value(From&& from) {
            _value = std::forward<From>(from);
            return {};
        }

        void return_void() {}
    };

    std::coroutine_handle<promise_type> _coro = nullptr;
    bool _full = false;

    Generator(std::coroutine_handle<promise_type> coro)
        : _coro(coro) {}

    Generator(Generator const& other) = delete;

    Generator(Generator&& other)
        : _coro(std::exchange(other._coro, nullptr)) {}

    Generator& operator=(Generator const& other) = delete;

    Generator& operator=(Generator&& other) {
        std::swap(_coro, other._coro);
        return *this;
    }

    ~Generator() {
        if (_coro)
            _coro.destroy();
    }

    void fill() {
        if (not _full) {
            _coro.resume();
            _full = true;
        }
    }

    explicit operator bool() {
        fill();
        return not _coro.done();
    }

    Opt<T> next() {
        fill();
        if (_coro.done())
            return NONE;
        _full = false;
        return _coro.promise()._value.take();
    }
};

static_assert(Iter2<Generator<int>>);

// MARK: Iter ------------------------------------------------------------------

template <Iter2 I>
struct It {
    using Item = decltype(Meta::declval<I>().next());
    Item curr;
    I& iter;

    constexpr auto& operator*() {
        return *curr;
    }

    constexpr auto operator++() {
        curr = iter.next();
        return *this;
    }

    constexpr bool operator!=(None) {
        return curr != NONE;
    }
};

export template <Iter2 I>
constexpr It<I> begin(I& iter) {
    return {iter.next(), iter};
}

export template <Iter2 I>
constexpr None end(I&) {
    return NONE;
}

// MARK: ForEach ---------------------------------------------------------------

template <typename F>
struct _ForEach {
    F f;
};

export template <typename F>
void operator|(auto&& iter, _ForEach<F> forEach) {
    while (auto value = iter.next())
        forEach.f(*value);
}

export template <typename F>
auto forEach(F f) {
    return _ForEach<F>{f};
}

// MARK: Collect ---------------------------------------------------------------

template <typename V>
struct _Collect {
};

export template <typename V>
V operator|(auto&& iter, _Collect<V>) {
    V v{};
    while (auto value = iter.next())
        v.pushBack(*value);
    return v;
}

export template <typename V>
auto collect() {
    return _Collect<V>{};
}

export auto collect() {
    return forEach([](auto const&...) {
    });
}

} // namespace Karm

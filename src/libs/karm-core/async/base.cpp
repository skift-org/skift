export module Karm.Core:async.base;

import :base.base;
import :meta.traits;

// Based on https://github.com/managarm/libasync
// Copyright 2016-2020 libasync Contributors

namespace Karm::Async {

export struct Inline {};

export constexpr Inline INLINE;

export struct Later {};

export constexpr Later LATER;

export struct InlineOrLater {
    constexpr InlineOrLater(Inline) {}

    constexpr InlineOrLater(Later) {}
};

export template <typename T>
concept Operation = requires(T t) {
    // return true if the operation is done immediately or false if it is pending
    { t.start() } -> Meta::Same<bool>;
};

export template <typename R, typename T>
concept Receiver = requires(R r, T t) {
    { r.recv(INLINE, t) };
    { r.recv(LATER, t) };
};

export template <typename T>
struct Null {
    void recv(auto, T) {}
};

static_assert(Receiver<Null<int>, int>);

export template <typename S>
concept Sender = requires(S s) {
    { s.connect(Null<typename S::Inner>{}) } -> Operation;
};

export template <Sender S, Receiver<typename S::Inner> R>
using OperationOf = decltype(Meta::declval<S>().connect(Meta::declval<R>()));

} // namespace Karm::Async

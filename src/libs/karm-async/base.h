#pragma once

// Based on https://github.com/managarm/libasync
// Copyright 2016-2020 libasync Contributors

#include <karm-base/base.h>
#include <karm-meta/traits.h>

namespace Karm::Async {

struct Inline {};

static constexpr Inline INLINE;

struct Later {};

static constexpr Later LATER;

struct InlineOrLater {
    constexpr InlineOrLater(Inline) {}

    constexpr InlineOrLater(Later) {}
};

template <typename T>
concept Operation = requires(T t) {
    // return true if the operation is done immediately or false if it is pending
    { t.start() } -> Meta::Same<bool>;
};

template <typename R, typename T>
concept Receiver = requires(R r, T t) {
    { r.recv(INLINE, t) };
    { r.recv(LATER, t) };
};

template <typename T>
struct Null {
    void recv(auto, T) {}
};

static_assert(Receiver<Null<int>, int>);

template <typename S>
concept Sender = requires(S s) {
    { s.connect(Null<typename S::Inner>{}) } -> Operation;
};

template <Sender S, Receiver<typename S::Inner> R>
using OperationOf = decltype(Meta::declval<S>().connect(Meta::declval<R>()));

} // namespace Karm::Async

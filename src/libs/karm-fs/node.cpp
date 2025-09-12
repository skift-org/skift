module;

#include <karm-sys/dir.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>

export module Karm.Fs:node;

import Karm.Ref;

namespace Karm::Fs {

export struct Node {
    Opt<Weak<Node>> _self;

    Node() {}

    virtual ~Node() = default;

    virtual Async::Task<> initAsync() {
        co_return Ok();
    }

    virtual Async::Task<Rc<Node>> createAsync(Str name, Sys::Type type) {
        (void)name;
        (void)type;
        co_return Error::readOnlyFilesystem();
    }

    virtual Async::Task<Rc<Node>> lookupAsync(Str name) {
        (void)name;
        co_return Error::notFound();
    }

    Async::Task<Rc<Node>> lookupAsync(Ref::Path const& path) {
        auto res = co_try$(_self.unwrap("node not self bound").upgrade());
        for (auto const& i : path.iter()) {
            res = co_trya$(lookupAsync(i));
        }
        co_return Ok(res);
    }

    virtual Async::Task<> linkAsync(Str name, Rc<Node> node) {
        (void)name;
        (void)node;
        co_return Error::readOnlyFilesystem();
    }

    virtual Async::Task<> unlinkAsync(Str name) {
        (void)name;
        co_return Error::readOnlyFilesystem();
    }

    virtual Async::Task<Serde::Object> inspectAsync() {
        co_return Ok(Serde::Object{});
    }

    virtual Async::Task<usize> readAsync(MutBytes buf, usize offset) {
        (void)buf;
        (void)offset;
        co_return Ok(0);
    }

    virtual Async::Task<usize> writeAsync(Bytes buf, usize offset) {
        (void)buf;
        (void)offset;
        co_return Ok(buf.len());
    }

    virtual Async::Task<> truncateAsync(usize len) {
        (void)len;
        co_return Ok();
    }

    virtual Async::Task<Rc<Node>> openAsync() {
        co_return Ok(co_try$(_self.unwrap("node not self bound").upgrade()));
    }

    virtual Async::Task<Vec<Sys::DirEntry>> listAsync() {
        co_return Ok<Vec<Sys::DirEntry>>();
    }

    virtual Async::Task<Sys::Stat> statAsync() {
        auto now = Sys::now();
        co_return Ok(Sys::Stat{
            .type = Sys::Type::OTHER,
            .size = 0,
            .accessTime = now,
            .modifyTime = now,
            .changeTime = now,
        });
    }

    virtual Async::Task<> syncAsync() {
        co_return Ok();
    }
};

export template <typename T, typename... Args>
Async::Task<Rc<T>> createAsync(Args&&... args) {
    auto node = makeRc<T>(std::forward<Args>(args)...);
    node->_self = node;
    co_trya$(node->initAsync());
    co_return Ok(node);
}

} // namespace Karm::Fs

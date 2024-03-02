#pragma once

#include "base.h"

namespace Karm::Ipc {

struct Transport {
    struct _Job {
        usize _id = 0;
        virtual ~_Job() = default;
        virtual void submit(Sys::IpcConnection &) = 0;
        virtual void complete(Sys::IpcConnection &) = 0;
    };
    Sys::IpcConnection _con;
    usize _id = 0;
    Map<usize, Strong<_Job>> _jobs;

    Transport(Sys::IpcConnection con)
        : _con(std::move(con)) {}

    void submit(Strong<_Job> job) {
        job->_id = _id++;
        _jobs.put(job->_id, std::move(job));
        job->submit(_con);
    }

    template <typename I, usize UID, typename Ret, typename... Args>
    Ret invoke(Args...) {
        struct Job : public _Job {
            void submit(Sys::IpcConnection &) override {}
            void complete(Sys::IpcConnection &) override {}
        };
        Io::BufferWriter reqData;
        Io::BEmit reqEmit{reqData};

        co_trya$(_con.sendAsync(reqData.bytes(), {}));

        co_return {};
    }
};

template <Interface T>
Async::Task<Strong<T>> connectAsync(Url::Url url) {
    auto con = co_try$(Sys::IpcConnection::connect(url));
    co_return Ok(makeStrong<typename T::template _Client<Transport>>(con));
}

} // namespace Karm::Ipc

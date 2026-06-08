#include <hal/mem.h>
#include <karm/macros>
#include <vaerk-handover/spec.h>

import Karm.Ipc;
import Karm.Logger;
import Karm.Ref;
import Karm.Sys;
import Karm.Sys.Skift;

import Abi.SysV;
import Hjert.Api;

import Strata.Protos;
import Strata.Cm;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

static constexpr bool DEBUG_COMPONENT = false;

namespace Strata::Cm {

struct ComponentManager {
    struct Component {
        ComponentManager& _cm;

        String _id;
        Sys::IpcConnection _conn;
        Hj::Job _job;

        Component(ComponentManager& cm, String id, Sys::IpcConnection conn, Hj::Job job)
            : _cm(cm), _id(id), _conn(std::move(conn)), _job(std::move(job)) {}

        Res<> _handleConnect(Ipc::Message& msg) {
            auto connect = try$(msg.unpack<ICm::Connect>());
            logDebugIf(DEBUG_COMPONENT, "'{}' requested connection to '{}'", _id, connect.url);
            return _cm.connect(connect);
        }

        Res<> _handleLaunch(Ipc::Message& msg) {
            auto [url] = try$(msg.unpack<ICm::Launch>());
            logDebugIf(DEBUG_COMPONENT, "'{}' requested launch of '{}'", _id, url);
            try$(_cm.start(url.host.str(), false));
            return Ok();
        }

        Async::Task<> runAsync(Async::CancellationToken ct) {
            logDebugIf(DEBUG_COMPONENT, "component '{}' attached", _id);
            while (true) {
                co_try$(ct.errorIfCanceled());
                auto msg = co_trya$(Ipc::recvAsync(_conn, ct));
                if (msg->is<ICm::Connect>())
                    (void)_handleConnect(*msg);
                else if (msg->is<ICm::Launch>()) {
                    (void)_handleLaunch(*msg);
                }
            }
        }

        template <typename T>
        Res<> notify(T const& payload) {
            return Ipc::send<T>(_conn, Ipc::SEQ_EVENT, payload);
        }

        Res<> incoming(ICm::Connect const& connect) {
            logDebugIf(DEBUG_COMPONENT, "notifying '{}' of incoming connection", _id);
            return notify(connect);
        }

        bool operator==(Component const& other) const {
            return this == &other;
        }
    };

    Map<String, Rc<Component>> _exported = {};
    Vec<Rc<Component>> _active;
    Async::Promise<> _exit;
    Async::Cancellation _cancellation;

    ComponentManager() {}

    ~ComponentManager() {
        _cancellation.cancel();
    }

    void shutdown(Rc<Component> component) {
        logDebugIf(DEBUG_COMPONENT, "shutting down component '{}'", component->_id);

        _exported.removeValue(component);
        _active.removeAll(component);

        if (_active.len() == 0) {
            logDebugIf(DEBUG_COMPONENT, "no active components, exiting...");
            _exit.resolve(Ok());
        }
    }

    Res<> start(Str id, bool exported) {
        logDebugIf(DEBUG_COMPONENT, "starting '{}' (exported: {})...", id, exported);

        auto [fd0, fd1] = try$(Sys::Skift::ChannelFd::create(id));
        auto job = try$(runElf(id, fd0));

        auto component = makeRc<Component>(*this, id, Sys::IpcConnection{{fd1, "ipc:"_url}}, std::move(job));
        _active.pushBack(component);

        if (exported) {
            _exported.put(id, component);
            logDebugIf(DEBUG_COMPONENT, "exposed '{}'", id);
        }

        Async::detach(component->runAsync(_cancellation.token()), [this, component](auto const&...) {
            shutdown(component);
        });

        return Ok();
    }

    Res<> connect(ICm::Connect const& connect) {
        auto component = try$(
            _exported.lookup(connect.url.scheme == "file" ? "strata-fs"s : connect.url.host.str())
                .okOr(Error::notFound("component not found"))
        );
        return component->incoming(connect);
    }

    Async::Task<> runAsync() {
        logInfo("initializing system services...");

        co_try$(start("strata-device"s, true));
        co_try$(start("strata-fs"s, true));
        co_try$(start("strata-input"s, true));
        co_try$(start("strata-shell"s, true));
        co_return co_await _exit.future();
    }
};

} // namespace Strata::Cm

Async::Task<> entryPointAsync(Sys::Env&, Async::CancellationToken) {
    co_try$(Hj::Task::self().label("strata-cm"));
    Strata::Cm::ComponentManager cm{};
    co_return co_await cm.runAsync();
}

void __panicHandler(PanicKind kind, char const* msg, usize len);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    registerPanicHandler(__panicHandler);

    Sys::Skift::globalPayload = reinterpret_cast<Handover::Payload*>(rawHandover);

    char const* argv[] = {"strata-cm", nullptr};
    char const* envp[] = {nullptr};
    Sys::Env env{
        1,
        argv,
        envp,
        "file:"_url,
    };
    Async::Cancellation cancellation;
    auto res = Sys::run(entryPointAsync(env, cancellation.token()));

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res.none().msg());
        self.crash().unwrap();
    }

    Abi::SysV::fini();
    self.ret().unwrap();
    unreachable();
}

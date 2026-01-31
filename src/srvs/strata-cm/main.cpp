#include <hjert-api/api.h>
#include <vaerk-handover/hook.h>

import Karm.Ref;
import Karm.Sys;
import Vaerk.Elf;
import Karm.Logger;
import Karm.Sys.Skift;
import Abi.SysV;

import Strata.Protos;

using namespace Karm;

static constexpr bool DEBUG_TASK = false;
static constexpr bool DEBUG_ELF = false;

namespace Strata::Bus {

static Res<Hj::Task> loadElf(Sys::Context& ctx, String id, Rc<Sys::Skift::DuplexFd> fd) {
    logInfo("activating service '{}'...", id);
    auto& handover = useHandover(ctx);
    auto bootfs = try$(Sys::Skift::Bootfs::ensure());

    logInfoIf(DEBUG_ELF, "mapping elf...");
    auto elfPath = Io::format("bundles/{}/_bin", id);
    auto [elfVmo, _] = try$(bootfs->openVmo(elfPath.str()));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logInfoIf(DEBUG_ELF, "creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    logInfoIf(DEBUG_ELF, "validating elf...");
    Vaerk::Elf::Image image{elfRange.bytes()};
    if (not image.valid())
        return Error::invalidInput("invalid elf");

    logInfoIf(DEBUG_ELF, "mapping the elf...");
    for (auto prog : image.programs()) {
        if (prog.type() != Vaerk::Elf::Program::LOAD)
            continue;

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);
        logInfoIf(DEBUG_ELF, "mapping section: {x}-{x}", prog.vaddr(), prog.vaddr() + size);
        if ((prog.flags() & Vaerk::Elf::ProgramFlags::WRITE) == Vaerk::Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, size, Hj::VmoFlags::UPPER));
            try$(sectionVmo.label("elf-writeable"));
            auto sectionRange = try$(Hj::map(sectionVmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
            auto sectionBytes = sectionRange.mutBytes();
            copy(prog.bytes(), sectionBytes);
            if (prog.memsz() > prog.filez()) {
                auto bss = mutNext(sectionBytes, prog.filez());
                zeroFill(bss);
            }
            try$(elfSpace.map(prog.vaddr(), sectionVmo, 0, size, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        } else {
            try$(elfSpace.map(prog.vaddr(), elfVmo, prog.offset(), size, {Hj::MapFlags::READ, Hj::MapFlags::EXEC}));
        }
    }

    logInfoIf(DEBUG_ELF, "mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, kib(64), Hj::VmoFlags::UPPER));
    try$(stackVmo.label("stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, 0, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));

    logInfoIf(DEBUG_TASK, "creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, domain, elfSpace));
    try$(task.label(id));

    logInfoIf(DEBUG_TASK, "mapping handover...");
    auto const* handoverRecord = handover.findTag(Handover::Tag::SELF);
    auto handoverVmo = try$(Hj::Vmo::create(Hj::ROOT, handoverRecord->start, handoverRecord->size, Hj::VmoFlags::DMA));
    try$(handoverVmo.label("handover"));
    auto handoverVrange = try$(elfSpace.map(0, handoverVmo, 0, 0, Hj::MapFlags::READ));

    logInfoIf(DEBUG_TASK, "attaching channels...");
    auto inCap = try$(domain.attach(fd->_in));
    auto outCap = try$(domain.attach(fd->_out));

    logInfoIf(DEBUG_TASK, "starting the task...");
    try$(task.start(
        image.header().entry,
        stackRange.end(),
        {
            handoverVrange.start,

            // NOTE: In and out are intentionally swapped
            outCap.slot(),
            inCap.slot(),
        }
    ));

    return Ok(std::move(task));
}

struct ComponentManager {
    struct Component {
        ComponentManager& _cm;

        String _id;
        Sys::IpcConnection _conn;
        Hj::Task _task;

        Component(ComponentManager& cm, String id, Sys::IpcConnection conn, Hj::Task task)
            : _cm(cm), _id(id), _conn(std::move(conn)), _task(std::move(task)) {}

        Res<> _handleConnect(Sys::IpcMessage& msg) {
            auto [fd, url] = try$(msg.unpack<ICm::Connect>());
            logInfo("'{}' requested connection to '{}'", _id, url);
            return _cm.connect(try$(fd.okOr(Error::invalidHandle("missing "))), url);
        }

        Res<> _handleLaunch(Sys::IpcMessage& msg) {
            auto [url] = try$(msg.unpack<ICm::Launch>());
            logInfo("'{}' requested launch of '{}'", _id, url);
            try$(_cm.start(url.host.str(), false));
            return Ok();
        }

        Async::Task<> runAsync(Async::CancellationToken ct) {
            logInfo("component '{}' attached", _id);
            while (true) {
                co_try$(ct.errorIfCanceled());
                auto msg = co_trya$(Sys::rpcRecvAsync(_conn, ct));
                if (msg.is<ICm::Connect>())
                    (void)_handleConnect(msg);
                else if (msg.is<ICm::Launch>()) {
                    (void)_handleLaunch(msg);
                }
            }
        }

        template <typename T>
        Res<> notify(T const& payload) {
            return rpcSend<T>(_conn, Sys::SEQ_EVENT, payload);
        }

        Res<> incoming(Rc<Sys::Fd> fd) {
            logInfo("notifying '{}' of incoming connection", _id);
            return notify(ICm::Incoming{fd});
        }

        bool operator==(Component const& other) const {
            return this == &other;
        }
    };

    Sys::Context& _context;
    Map<String, Rc<Component>> _exported = {};
    Vec<Rc<Component>> _active;
    Async::Promise<> _exit;
    Async::Cancellation _cancellation;

    ComponentManager(Sys::Context& ctx) : _context(ctx) {}

    ~ComponentManager() {
        _cancellation.cancel();
    }

    void shutdown(Rc<Component> component) {
        logInfo("shutting down component '{}'", component->_id);

        _exported.removeAll(component);
        _active.removeAll(component);

        if (_active.len() == 0) {
            logInfo("no active components, exiting...");
            _exit.resolve(Ok());
        }
    }

    Res<> start(Str id, bool exported) {
        logInfo("starting '{}' (exported: {})...", id, exported);

        auto fd = try$(Sys::Skift::DuplexFd::create(id));
        auto task = try$(loadElf(_context, id, fd));

        auto component = makeRc<Component>(*this, id, Sys::IpcConnection{fd, "ipc:"_url}, std::move(task));
        _active.pushBack(component);

        if (exported) {
            _exported.put(id, component);
            logDebug("exposed '{}'", id);
        }

        Async::detach(component->runAsync(_cancellation.token()), [this, component](auto const&...) {
            shutdown(component);
        });

        return Ok();
    }

    Res<> connect(Rc<Sys::Fd> fd, Ref::Url url) {
        auto component = try$(
            _exported.tryGet(url.host.str())
                .okOr(Error::notFound("component not found"))
        );
        return component->incoming(fd);
    }

    Async::Task<> runAsync() {
        logInfo("initializing system services...");

        co_try$(start("strata-device"s, true));
        // co_try$(start("strata-fs"s, true));
        co_try$(start("strata-input"s, true));
        co_try$(start("strata-shell"s, true));
        co_return co_await _exit.future();
    }
};

} // namespace Strata::Bus

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken) {
    co_try$(Hj::Task::self().label("strata-cm"));
    Strata::Bus::ComponentManager cm{ctx};
    co_return co_await cm.runAsync();
}

void __panicHandler(Karm::PanicKind kind, char const* msg);

extern "C" void __entryPoint(usize rawHandover) {
    Abi::SysV::init();
    Karm::registerPanicHandler(__panicHandler);

    Sys::Context ctx;
    char const* argv[] = {"strata-cm", nullptr};
    ctx.add<Sys::ArgsHook>(1, argv);
    ctx.add<HandoverHook>((Handover::Payload*)rawHandover);

    Async::Cancellation cancellation;
    auto res = Sys::run(entryPointAsync(ctx, cancellation.token()));

    auto self = Hj::Task::self();

    if (not res) {
        logError("{}: {}", argv[0], res.none().msg());
        self.crash().unwrap();
    }

    Abi::SysV::fini();
    self.ret().unwrap();
    unreachable();
}

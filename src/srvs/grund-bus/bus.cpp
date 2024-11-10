#include <elf/image.h>
#include <handover/hook.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>

#include "bus.h"

namespace Grund::Bus {

static constexpr bool DEBUG_TASK = false;
static constexpr bool DEBUG_ELF = false;

// MARK: Service ---------------------------------------------------------------

Res<Strong<Service>> Service::prepare(Sys::Context &, Str id) {
    logInfo("prepare service '{}'", id);

    auto in = try$(Hj::Channel::create(Hj::Domain::self(), 4096, 16));
    try$(in.label(Io::format("{}-in", id).unwrap()));

    auto out = try$(Hj::Channel::create(Hj::Domain::self(), 512, 16));
    try$(out.label(Io::format("{}-out", id).unwrap()));

    auto ipc = makeStrong<Skift::IpcFd>(
        std::move(in),
        std::move(out)
    );

    return Ok(makeStrong<Service>(id, ipc));
}

Res<> Service::activate(Sys::Context &ctx) {
    logInfo("activating service '{}'...", _id);

    auto &handover = useHandover(ctx);
    auto urlStr = try$(Io::format("bundle://{}/_bin", _id));
    auto *elf = handover.fileByName(urlStr.buf());
    if (not elf)
        return Error::invalidFilename("service not found");

    logInfoIf(DEBUG_ELF, "mapping elf...");
    auto elfVmo = try$(Hj::Vmo::create(Hj::ROOT, elf->start, elf->size, Hj::VmoFlags::DMA));
    try$(elfVmo.label("elf-shared"));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logInfoIf(DEBUG_ELF, "creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    logInfoIf(DEBUG_ELF, "validating elf...");
    Elf::Image image{elfRange.bytes()};
    if (not image.valid())
        return Error::invalidInput("invalid elf");

    logInfoIf(DEBUG_ELF, "mapping the elf...");
    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD)
            continue;

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);
        logInfoIf(DEBUG_ELF, "mapping section: {x}-{x}", prog.vaddr(), prog.vaddr() + size);
        if ((prog.flags() & Elf::ProgramFlags::WRITE) == Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, size, Hj::VmoFlags::UPPER));
            try$(sectionVmo.label("elf-writeable"));
            auto sectionRange = try$(Hj::map(sectionVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(elfSpace.map(prog.vaddr(), sectionVmo, 0, size, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(elfSpace.map(prog.vaddr(), elfVmo, prog.offset(), size, Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfoIf(DEBUG_ELF, "mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, kib(64), Hj::VmoFlags::UPPER));
    try$(stackVmo.label("stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfoIf(DEBUG_TASK, "creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, domain, elfSpace));
    try$(task.label(_id));

    logInfoIf(DEBUG_TASK, "mapping handover...");
    auto const *handoverRecord = handover.findTag(Handover::Tag::SELF);
    auto handoverVmo = try$(Hj::Vmo::create(Hj::ROOT, handoverRecord->start, handoverRecord->size, Hj::VmoFlags::DMA));
    try$(handoverVmo.label("handover"));
    auto handoverVrange = try$(elfSpace.map(0, handoverVmo, 0, 0, Hj::MapFlags::READ));

    logInfoIf(DEBUG_TASK, "attaching channels...");
    auto inCap = try$(domain.attach(_ipc->_in));
    auto outCap = try$(domain.attach(_ipc->_out));

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

    _task = std::move(task);

    return Ok();
}

Async::Task<> Service::runAsync() {
    logDebug("Listening for messages on service '{}'...", _id);
    while (true) {
        auto msg = co_trya$(Sys::ipcRecvAsync(_con));

        logDebug("Received message on service '{}'", _id);

        auto maybeHeader = msg.header();
        if (not maybeHeader)
            continue;
        auto &header = maybeHeader.unwrap();
        auto res = _bus->dispatch(maybeHeader.unwrap(), msg);
        if (not res)
            co_try$(Sys::ipcSend<Error>(_con, header.port, header.seq, res.none()));
    }
}

Res<> Service::dispatch(Sys::Message &msg) {
    logDebug("Dispatching message on service '{}'", _id);
    return _con.send(sub(msg.bytes, msg.len), sub(msg.handles, msg.handlesLen));
}

// MARK: Bus -------------------------------------------------------------------

Karm::Res<Strong<Bus>> Bus::create(Sys::Context &ctx) {
    return Ok(makeStrong<Bus>(ctx));
}

Karm::Res<> Bus::startService(Str id) {
    auto service = try$(Service::prepare(_context, id));
    try$(attach(service));
    Async::detach(service->runAsync());
    return Ok();
}

Karm::Res<> Bus::attach(Strong<Endpoint> endpoint) {
    endpoint->attach(*this);
    _endpoints.pushBack(endpoint);
    return Ok();
}

Res<> Bus::dispatch(Sys::Header &h, Sys::Message &msg) {
    logDebug("dispatching message to {}", h.port);
    for (auto &endpoint : _endpoints) {
        if (endpoint->port() == h.port)
            return endpoint->dispatch(msg);
    }
    return Error::notFound("service not found");
}

} // namespace Grund::Bus

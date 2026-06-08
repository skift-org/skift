module;

#include <karm/macros>
#include <vaerk-handover/spec.h>

export module Strata.Cm:runner;

import Karm.Dl.Elf;
import Karm.Sys;
import Karm.Logger;
import Karm.Sys.Skift;
import Hjert.Api;

using namespace Karm;
using namespace Karm::Literals;

namespace Strata::Cm {

static constexpr bool DEBUG_ELF = false;
static constexpr bool DEBUG_TASK = false;

export Res<Hj::Job> runElf(String id, Rc<Sys::Skift::ChannelFd> fd) {
    auto& handover = Sys::Skift::useHandover();
    auto bootfs = try$(Sys::Skift::Bootfs::ensure());

    logDebugIf(DEBUG_ELF, "mapping elf...");
    auto elfPath = Io::format("bundles/{}/bin/{}.elf", id, id);
    auto [elfVmo, _] = try$(bootfs->openVmo(elfPath.str()));
    auto elfRange = try$(Hj::map(elfVmo, Hj::MapFlags::READ));

    logDebugIf(DEBUG_ELF, "creating address space...");
    auto elfSpace = try$(Hj::Space::create(Hj::ROOT));

    logDebugIf(DEBUG_ELF, "validating elf...");
    Elf::ElfObject<Elf::CurrentAbi> object{elfRange.bytes()};
    try$(object.validate());

    logDebugIf(DEBUG_ELF, "mapping the elf...");
    for (Elf::ElfProgram prog : object.iterProgram()) {
        if (prog.type() != Elf::ElfPhdrType::PT_LOAD)
            continue;

        usize size = alignUp(max(prog.p_memsz, prog.p_filesz), Sys::pageSize());
        logDebugIf(DEBUG_ELF, "mapping section: {x}-{x}", prog.p_vaddr, prog.p_vaddr + size);

        if (prog.p_flags.has(Elf::ElfPhdrFlags::PF_W)) {
            auto sectionVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, size, Hj::VmoFlags::UPPER));
            try$(sectionVmo.label("elf-writeable"));
            auto sectionRange = try$(Hj::map(sectionVmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
            auto sectionBytes = sectionRange.mutBytes();
            copy(prog.data, sectionBytes);
            if (prog.p_memsz > prog.p_filesz) {
                auto bss = mutNext(sectionBytes, prog.p_filesz);
                zeroFill(bss);
            }
            try$(elfSpace.map(prog.p_vaddr, sectionVmo, 0, size, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));
        } else {
            try$(elfSpace.map(prog.p_vaddr, elfVmo, prog.p_offset, size, {Hj::MapFlags::READ, Hj::MapFlags::EXEC}));
        }
    }

    logDebugIf(DEBUG_ELF, "mapping the stack...");
    auto stackVmo = try$(Hj::Vmo::create(Hj::ROOT, 0, 64_KiB, Hj::VmoFlags::UPPER));
    try$(stackVmo.label("stack"));
    auto stackRange = try$(elfSpace.map(0, stackVmo, 0, 0, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));

    logDebugIf(DEBUG_TASK, "creating the task...");
    auto domain = try$(Hj::Domain::create(Hj::ROOT));
    auto job = try$(Hj::Job::create(Hj::ROOT, Hj::ROOT));
    auto task = try$(Hj::Task::create(Hj::ROOT, job, domain, elfSpace));
    try$(task.label(id));

    logDebugIf(DEBUG_TASK, "mapping handover...");
    auto const* handoverRecord = handover.findTag(Handover::Tag::SELF);
    auto handoverVmo = try$(Hj::Vmo::create(Hj::ROOT, handoverRecord->start, handoverRecord->size, Hj::VmoFlags::DMA));
    try$(handoverVmo.label("handover"));
    auto handoverVrange = try$(elfSpace.map(0, handoverVmo, 0, 0, Hj::MapFlags::READ));

    logDebugIf(DEBUG_TASK, "attaching channels...");
    auto inCap = try$(domain.attach(fd->_in));
    auto outCap = try$(domain.attach(fd->_out));

    logDebugIf(DEBUG_TASK, "starting the task...");
    try$(task.start(
        object.header().e_entry,
        stackRange.end(),
        {
            handoverVrange.start,
            inCap.slot(),
            outCap.slot(),
        }
    ));

    return Ok(std::move(job));
}

} // namespace Strata::Cm
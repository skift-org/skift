#pragma once

#include <ce-bootfs/bootfs.h>
#include <hjert-api/api.h>
#include <vaerk-handover/hook.h>

struct Bootfs {
    uintptr_t _physStart;
    Hj::Vmo _vmo;
    Hj::Mapped _range;

    bootfs_header_t* header() {
        return reinterpret_cast<bootfs_header_t*>(_range.mutBytes().buf());
    }

    static Res<Bootfs*> ensure(Sys::Context& ctx = Sys::rootContext()) {
        static Opt<Bootfs> bootfs;

        if (bootfs)
            return Ok(&bootfs.unwrap());

        auto& handover = useHandover(ctx);

        auto* bootfsRecord = handover.fileByName("file:/skift/init.bootfs");
        if (not bootfsRecord)
            return Error::notFound("no bootfs");

        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, bootfsRecord->start, bootfsRecord->size, Hj::VmoFlags::DMA));
        try$(vmo.label("bootfs"));

        auto range = try$(Hj::map(vmo, Hj::MapFlags::READ));

        Bootfs tmp{
            ._physStart = bootfsRecord->start,
            ._vmo = std::move(vmo),
            ._range = std::move(range),
        };
        bootfs = std::move(tmp);
        return Ok(&bootfs.unwrap());
    }

    Res<bootfs_dirent_t*> openDirent(Str path) {
        auto* fileDirent = bootfs_open(header(), path.buf(), path.len()).dirent;
        if (not fileDirent)
            return Error::notFound("elf not found");
        return Ok(fileDirent);
    }

    Res<Hj::Vmo> openVmo(Str path) {
        auto* fileDirent = bootfs_open(header(), path.buf(), path.len()).dirent;
        if (not fileDirent)
            return Error::notFound("elf not found");

        auto fileVmo = try$(
            Hj::Vmo::create(
                Hj::ROOT,
                _physStart + fileDirent->offset,
                alignUp(fileDirent->length, Sys::pageSize()),
                Hj::VmoFlags::DMA
            )
        );
        try$(fileVmo.label(path));

        return Ok(std::move(fileVmo));
    }
};

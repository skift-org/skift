module;

#include <karm-base/rc.h>
#include <karm-crypto/crc32.h>
#include <karm-logger/logger.h>

export module Karm.Kv:store;

import :blob;
import :wal;

namespace Karm::Kv {

export struct Store {
    Rc<Wal> _wal;
    Map<Blob, Blob> _memdb;

    static Rc<Store> open(Rc<Wal> wal) {
        auto db = makeRc<Store>(wal);
        for (Wal::Record const& r : wal->iter()) {
            if (r.type == Wal::PUT) {
                db->_memdb.put(r.key, r.value);
            } else if (r.type == Wal::DEL) {
                db->_memdb.del(r.key);
            }
        }
        return db;
    };

    Res<> put(Bytes key, Bytes value) {
        try$(_wal->record(Wal::PUT, key, value));
        _memdb.put(Blob::from(key), Blob::from(value));
        return Ok();
    }

    Res<> del(Bytes key) {
        try$(_wal->record(Wal::DEL, key, {}));
        _memdb.del(Blob::from(key));
        return Ok();
    }
};

} // namespace Karm::Kv

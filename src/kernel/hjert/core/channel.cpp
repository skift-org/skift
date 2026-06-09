module;

#include <karm/macros>

export module Hjert.Core:channel;

import Karm.Core;
import :domain;

namespace Hjert::Core {

export struct Channel : BaseObject<Channel, Hj::Type::CHANNEL> {
    enum Side {
        IN,
        OUT,
    };

    struct State {
        Lock _lock;
        Ring<Hj::SentRecv> _sr;
        Ring<u8> _bytes;
        Ring<Arc<Object>> _caps;
        usize _in = 0;
        usize _out = 0;

        State(usize bufCap, usize capsCap)
            : _sr(max(bufCap / 16, 16uz)),
              _bytes(bufCap),
              _caps(capsCap) {
        }

        void open(Side side) {
            LockScope _{_lock};
            (side == OUT ? _out : _in)++;
        }

        void close(Side side) {
            LockScope _{_lock};
            (side == OUT ? _out : _in)--;
        }

        bool closed() const {
            return _out == 0 or _in == 0;
        }

        Res<> ensureOpen() {
            if (closed())
                return Error::brokenPipe("channel closed");
            return Ok();
        }

        Res<Hj::SentRecv> send(Domain& dom, Bytes bytes, Slice<Hj::Cap> caps) {
            LockScope _{_lock};

            try$(ensureOpen());

            // Make sure everything is ready for the message
            if (_sr.rem() < 1)
                return Error::invalidInput("not enough space for message");

            if (_bytes.rem() < bytes.len())
                return Error::invalidInput("not enough space for bytes");

            if (_caps.rem() < caps.len())
                return Error::invalidInput("not enough space for caps");

            // Everything is ready, let's send the message
            auto save = _caps.len();
            for (auto cap : caps) {
                auto res = dom.get(cap);
                if (not res) {
                    // Uh oh, we need to rollback
                    _caps.trunc(save);
                    return res.none();
                }
                _caps.pushBack(res.unwrap());
            }

            for (usize i = 0; i < bytes.len(); i++)
                _bytes.pushBack(bytes[i]);

            _sr.pushBack({bytes.len(), caps.len()});

            return Ok<Hj::SentRecv>(bytes.len(), caps.len());
        }

        Res<Hj::SentRecv> recv(Domain& dom, MutBytes bytes, MutSlice<Hj::Cap> caps) {
            LockScope _{_lock};

            ObjectLockScope domScope{dom};

            if (_sr.len() == 0) {
                if (_out == 0)
                    return Error::brokenPipe("channel closed");
                return Error::wouldBlock("no messages available");
            }

            auto& [expectedBytes, expectedCaps] = _sr.peekFront(0);
            if (bytes.len() < expectedBytes)
                return Error::invalidInput("not enough space for bytes");

            if (caps.len() < expectedCaps)
                return Error::invalidInput("not enough space for caps");

            if (dom._availableUnlocked() < expectedCaps)
                return Error::invalidInput("not enough space in domain");

            // Everything is ready, let's receive the message
            _sr.popFront();

            for (usize i = 0; i < expectedBytes; i++)
                bytes[i] = _bytes.popFront();

            for (usize i = 0; i < expectedCaps; i++) {
                // NOTE: We unwrap here because we know that the domain has enough space
                caps[i] = dom._addUnlock(Hj::ROOT, _caps.popFront()).unwrap("domain full");
            }

            return Ok<Hj::SentRecv>(expectedBytes, expectedCaps);
        }

        Flags<Hj::Sigs> poll(Side side) {
            LockScope _{_lock};

            bool readable = _sr.len() > 0;
            bool writable = _sr.rem() > 0;

            Flags sigs = Hj::Sigs::NONE;

            if (readable)
                sigs |= Hj::Sigs::READABLE;

            if (writable)
                sigs |= Hj::Sigs::WRITABLE;

            if (side == IN) {
                if (_out == 0 and not readable)
                    sigs |= Hj::Sigs::CLOSED;
            } else {
                if (_in == 0)
                    sigs |= Hj::Sigs::CLOSED;
            }

            return sigs;
        }
    };

    Arc<State> _state;
    Side _side;

    static Res<Pair<Arc<Channel>>> create(usize bufCap = 256_KiB, usize capsCap = 512) {
        auto state = makeArc<State>(bufCap, capsCap);
        return Ok(Pair{
            makeArc<Channel>(state, IN),
            makeArc<Channel>(state, OUT),
        });
    }

    Channel(Arc<State> state, Side side)
        : _state(state), _side(side) {
        _state->open(_side);
    }

    ~Channel() {
        _state->close(_side);
    }

    Res<Hj::SentRecv> send(Domain& dom, Bytes bytes, Slice<Hj::Cap> caps) {
        return _state->send(dom, bytes, caps);
    }

    Res<Hj::SentRecv> recv(Domain& dom, MutBytes bytes, MutSlice<Hj::Cap> caps) {
        return _state->recv(dom, bytes, caps);
    }

    Flags<Hj::Sigs> _pollUnlock() override {
        auto base = _signals & ~Flags{Hj::Sigs::READABLE, Hj::Sigs::WRITABLE, Hj::Sigs::CLOSED};
        return _state->poll(_side) | base;
    }
};

} // namespace Hjert::Core
#pragma once

#include <karm-base/endian.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>
#include <karm-sys/socket.h>

namespace Karm::Net::Dns {

static u16 const PORT = 53;
static usize const MAX_NAME_LEN = 253;
static usize const MAX_LABEL_LEN = 63;

static Sys::SocketAddr const GOOGLE = {Sys::Ip4{8, 8, 8, 8}, PORT};
static Sys::SocketAddr const CLOUDFLARE = {Sys::Ip4{1, 1, 1, 1}, PORT};

enum Type {
    A = 1,
    NS = 2,
    CNAME = 5,
    SOA = 6,
    PTR = 12,
    MX = 15,
    TXT = 16,
    AAAA = 28,
    SRV = 33,
};

enum Class {
    IN = 1,
    CH = 3,
    HS = 4,
};

enum Flags : u16 {
    RCODE = 0xF << 0,
    Z = 0b111 << 4,
    RA = 1 << 7,
    RD = 1 << 8,
    TC = 1 << 9,
    AA = 1 << 10,
    OPCODE = 0xF << 11,
    QR = 1 << 15,
};

enum OpCode : u16 {
    QUERY = 0 << 1,
    IQUERY = 1 << 1,
    STATUS = 2 << 1,
    NOTIFY = 4 << 1,
    UPDATE = 5 << 1,
};

#define FOREACH_RCODE(RCODE)  \
    RCODE(NO_ERROR, 0)        \
    RCODE(FORMAT_ERROR, 1)    \
    RCODE(SERVER_FAILURE, 2)  \
    RCODE(NAME_ERROR, 3)      \
    RCODE(NOT_IMPLEMENTED, 4) \
    RCODE(REFUSED, 5)         \
    RCODE(YXDOMAIN, 6)        \
    RCODE(XRRSET, 7)          \
    RCODE(NOTAUTH, 8)         \
    RCODE(NOTZONE, 9)

enum RCode : u16 {
#define ITER(NAME, VAL) NAME = VAL << 12,
    FOREACH_RCODE(ITER)
#undef ITER
};

Str toStr(RCode code) {
    switch (code) {
#define ITER(NAME, VAL) \
    case RCode::NAME:   \
        return #NAME;
        FOREACH_RCODE(ITER)
#undef ITER
    default:
        return "UNKNOWN";
    }
}

struct Header {
    u16be id;
    u16be flags;
    u16be qdcount;
    u16be ancount;
    u16be nscount;
    u16be arcount;

    OpCode opcode() const {
        return (OpCode)((u16)flags & (u16)Flags::OPCODE);
    }

    RCode rcode() const {
        return (RCode)((u16)flags & (u16)Flags::RCODE);
    }

    Bytes bytes() const {
        return {(Byte const *)this, sizeof(*this)};
    }
};

struct Question {
    String name;
    Type type;
    Class class_;
};

struct Answer {
    String name;
    Type type;
    Class class_;
    TimeSpan ttl;
    Buf<Byte> data;
};

Res<> encodeName(Io::BEmit &e, Str name) {
    for (auto part : iterSplit(name, '.')) {
        e.writeU8be(part.len());
        e.writeStr(part);
    }
    e.writeU8be(0);
    return Ok();
}

Res<usize> decodeName(Cursor<Byte> const start, Cursor<Byte> curr, StringBuilder &out) {
    usize len = 0;
    while (not curr.ended()) {
        auto b = curr.next();
        len++;

        if (b == 0) {
            // End of name
            if (out.len() > MAX_NAME_LEN)
                return Error::invalidData("dns name too long");

            return Ok(len);
        } else if ((b & 0xc0) == 0xc0) {
            // Pointer
            if (curr.ended())
                return Error::unexpectedEof("unexpected end of dns name");
            usize off = ((b & 0x3f) << 8) | curr.next();
            len++;
            if (off >= start.len())
                return Error::invalidData("invalid dns name pointer");
            auto next = start;
            next.next(off);
            try$(decodeName(start, next, out));
            return Ok(len);
        } else {
            // Label
            if (b > MAX_LABEL_LEN)
                return Error::invalidData("dns label too long");
            if (out.len() + b + 1 > MAX_NAME_LEN)
                return Error::invalidData("dns name too long");
            if (out.len() > 0)
                out.append('.');
            out.append(curr.next(b).cast<char>());
            len += b;
        }
    }

    return Error::unexpectedEof("unexpected end of dns name");
}

struct Packet {
    usize _id;
    Flags _flags;
    Vec<Question> _qs;
    Vec<Answer> _ans;

    Packet() = default;

    Packet(usize id, Flags flags)
        : _id(id),
          _flags(flags) {}

    Header header() const {
        Header hdr;
        hdr.id = 420;
        hdr.flags = _flags;
        hdr.qdcount = _qs.len();
        hdr.ancount = _ans.len();
        hdr.nscount = 0;
        hdr.arcount = 0;
        return hdr;
    }

    static Res<Buf<Byte>> encode(Packet const &p) {
        Io::BufferWriter buf;
        Io::BEmit e(buf);

        e.writeBytes(p.header().bytes());

        for (auto &q : p._qs) {
            try$(encodeName(e, q.name));
            e.writeU16be(q.type);
            e.writeU16be(q.class_);
        }

        for (auto &a : p._ans) {
            try$(encodeName(e, a.name));
            e.writeU16be(a.type);
            e.writeU16be(a.class_);
            e.writeU32be(a.ttl.toSecs());
            e.writeU16be(a.data.len());
            e.writeBytes(a.data);
        }

        return Ok(buf.take());
    }

    static Res<Packet> decode(Bytes buf) {
        Io::BScan s{buf};

        if (s.rem() < sizeof(Header))
            return Error::unexpectedEof("packet too short");

        Header hdr;
        s.readTo(&hdr);

        Packet pkt{hdr.id, (Flags)hdr.flags.value()};
        Vec<Question> &qs = pkt._qs;
        for (auto i = 0; i < hdr.qdcount; i++) {
            StringBuilder name;
            s.skip(try$(decodeName(buf, s.remBytes(), name)));

            Question q{
                name.take(),
                (Type)s.nextU16be(),
                (Class)s.nextU16be(),
            };

            qs.pushBack(std::move(q));
        }

        Vec<Answer> &ans = pkt._ans;
        for (auto i = 0; i < hdr.ancount; i++) {
            StringBuilder name;
            s.skip(try$(decodeName(buf, s.remBytes(), name)));

            Answer a;
            a.name = name.take();
            a.type = (Type)s.nextU16be();
            a.class_ = (Class)s.nextU16be();
            a.ttl = TimeSpan::fromSecs(s.nextU32be());
            auto len = s.nextU16be();
            a.data = s.nextBytes(len);

            ans.pushBack(std::move(a));
        }

        return Ok(std::move(pkt));
    }
};

struct Client {
    Sys::SocketAddr _server;
    Sys::UdpConnection _conn;

    Client(Sys::SocketAddr server, Sys::UdpConnection conn)
        : _server(server),
          _conn(std::move(conn)) {}

    static Res<Client> connect(Sys::SocketAddr server = GOOGLE) {
        auto conn = try$(Sys::UdpConnection::listen({Sys::Ip4::unspecified(), 0}));
        return Ok(Client(server, std::move(conn)));
    }

    Res<Vec<Answer>> ask(Slice<Question> qs) {
        Packet req{420, Flags::RD};
        req._qs = qs;

        logDebug("sending dns request to {}", _server);
        try$(_conn.send(try$(Packet::encode(req)), _server));

        logDebug("waiting for dns response");
        Array<Byte, 4096> buf;
        auto [len, addr] = try$(_conn.recv(buf));

        logDebug("received dns response from {}", addr);
        if (addr != _server)
            return Error::invalidData("received response from wrong address");

        Packet resp = try$(Packet::decode(sub(buf, 0, len)));

        if (resp._id != req._id)
            return Error::invalidData("received response with wrong id");

        if (resp.header().rcode() != RCode::NO_ERROR)
            return Error::invalidData("received response with error code");

        return Ok(std::move(resp._ans));
    }

    Res<Sys::Ip> resolve(Str host) {
        auto qs = Vec<Question>{};
        qs.pushBack(Question{host, Type::A, Class::IN});

        auto answers = try$(ask(qs));
        if (answers.len() == 0)
            return Error::invalidData("no answers");

        auto ans = answers[0];
        if (ans.type != Type::A)
            return Error::invalidData("invalid answer type");

        if (ans.data.len() != 4)
            return Error::invalidData("invalid answer data");

        Sys::Ip4 ip{
            ans.data[0],
            ans.data[1],
            ans.data[2],
            ans.data[3],
        };

        return Ok(ip);
    }
};

} // namespace Karm::Net::Dns

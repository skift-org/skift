#pragma once

#include <karm-base/distinct.h>
#include <karm-base/slice.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>
#include <karm-sys/socket.h>

// https://github.com/B-Con/crypto-algorithms/blob/master/aes.c
// https://github.com/alexbers/manual-tls/blob/master/tls_client.py

namespace Web::Tls {

struct CipherSuite : public Distinct<u16be, struct _CipherSuite> {
    using Distinct::Distinct;
    static CipherSuite TLS_AES_128_GCM_SHA256;
};

inline CipherSuite CipherSuite::TLS_AES_128_GCM_SHA256{0x1301};

enum struct ContentType : u8 {
    CHANGE_CIPHER_SPEC = 0x14,
    ALERT = 0x15,
    HANDSHAKE = 0x16,
    APPLICATION_DATA = 0x17,
};

static bool
isHello(Bytes buf) {
    return buf.len() >= 5 and
           buf[0] == 0x16 and
           buf[1] == 0x03 and
           buf[2] <= 0x03 and
           buf[5] == 0x01;
}

struct TlsConnection : public Sys::_Connection {
    Sys::_Connection &_con;

    static Res<TlsConnection> accept(Sys::_Connection &con, Bytes hello) {
        if (not isHello(hello))
            return Error::connectionRefused("not a tls client hello");

        Io::BScan s{hello};

        auto tls = TlsConnection(con);

        return Ok(std::move(tls));
    }

    static Res<TlsConnection> connect(Sys::TcpConnection &con) {
        auto tls = TlsConnection(con);

        return Ok(std::move(tls));
    }

    TlsConnection(Sys::_Connection &con)
        : _con(con) {}

    Res<usize> read(MutBytes buf) override {
        return _con.read(buf);
    }

    Res<usize> write(Bytes buf) override {
        return _con.write(buf);
    }

    Res<usize> flush() override {
        return _con.flush();
    }
};

} // namespace Web::Tls

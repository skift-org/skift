#pragma once
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libutils/ResultOr.h>

class AudioEngine
{
public:
    AudioEngine();

private:
    void send_message(const MixerMessage &);
    void do_message(const MixerMessage &);
    ResultOr<MixerMessage> wait_for_message(MixerMessageType expected_message);

    Result _error;
    Connection *_connection;
    OwnPtr<Notifier> _connection_notifier;
};
#pragma once

namespace protocol
{

template <typename Protocol>
class Client
{
private:
public:
    Client() {}
    ~Client() {}

    virtual void handle_signal(Protocol::ServerSignal signal);
};

} // namespace protocol

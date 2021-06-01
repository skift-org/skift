#include <abi/Paths.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <string.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    IO::File network_device{NETWORK_DEVICE_PATH, HJ_OPEN_READ | HJ_OPEN_WRITE};

    if (argc == 2 && strcmp(argv[1], "-i") == 0)
    {
        IOCallNetworkSateAgs state = {};

        network_device.handle()->call(IOCALL_NETWORK_GET_STATE, &state);

        IO::outln("MAC: {02x}:{02x}:{02x}:{02x}:{02x}:{02x}",
                  state.mac_address[0],
                  state.mac_address[1],
                  state.mac_address[2],
                  state.mac_address[3],
                  state.mac_address[4],
                  state.mac_address[5]);
    }

    return 0;
}

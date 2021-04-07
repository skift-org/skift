#include <libsystem/io/Stream.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Stream *network_device = stream_open(NETWORK_DEVICE_PATH, OPEN_READ | OPEN_WRITE);

    if (argc == 2 && strcmp(argv[1], "-i") == 0)
    {
        IOCallNetworkSateAgs state = {};

        stream_call(network_device, IOCALL_NETWORK_GET_STATE, &state);

        printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               state.mac_address[0], state.mac_address[1], state.mac_address[2], state.mac_address[3], state.mac_address[4], state.mac_address[5]);
    }

    stream_close(network_device);

    return 0;
}

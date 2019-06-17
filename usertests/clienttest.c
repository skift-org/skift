#include <skift/messaging.h>
#include <skift/logger.h>
#include <hideo/client.h>
#include <hideo/server.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    if (argc >= 2)
    {
        int hideo_server_pid = stoi(argv[1], 10);

        message_t client_request = message(HIDEO_CLIENT_HELLO, hideo_server_pid);
        message_t server_respond;
        message_set_payload(client_request, (hideo_client_hello_t){"Test client application"});
        if (messaging_request(&client_request, &server_respond, 100) == 0)
        {
            logger_log(LOG_DEBUG, "Server respond %s", server_respond.header.label);
        }
        else
        {
            
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

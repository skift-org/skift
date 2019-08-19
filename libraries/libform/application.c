/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/logger.h>
#include <libsystem/messaging.h>
#include <libsystem/cmdline.h>

#include <libform/application.h>
#include <libform/wm/server.h>
#include <libform/wm/client.h>

static bool application_running = false;
static bool application_initialized = false;
static list_t *application_windows = NULL;
static int application_exit_value = 0;
static int application_server_pid = -1;

void application_init(int argc, char **argv, const char *title)
{
    UNUSED(argc);
    UNUSED(argv);

    assert(!application_initialized);
    assert(!application_running);

    // Get the pid of the server
    // iostream_t* fpid = iostream_open("/run/hideo_pid", IOSTREAM_READ);
    // if (fpid)
    // {
    //     // FIXME: we need scanf
    // }
    // else
    // {
    //     logger_error("Failled to open /run")
    // }

    // Connect to the server
    if (application_send_request(HIDEO_CLIENT_HELLO, title, strnlen(title, MSGPAYLOAD_SIZE)) == 0)
    {
        application_windows = list();
        application_initialized = true;

        logger_info("Application '%s' initialized!");
    }
    else
    {
        logger_error("Failled to say hello to the hideo server!");
    }
}

int application_run(void)
{
    assert(application_initialized);
    assert(!application_running);

    application_running = true;

    logger_info("Entering event loop...");

    do
    {
        // Get the message

        // Is
    } while (application_running);

    logger_info("Event loop exited!");

    return application_exit_value;
}

void application_quit(int exit_value)
{
    assert(application_initialized);
    assert(application_running);

    application_running = false;
    application_exit_value = exit_value;

    logger_info("Application quit!");
}

int application_get_server_pid(void)
{
    assert(application_initialized);

    return application_server_pid;
}

int application_send_request(const char *request, const void *payload, int payload_size)
{
    message_t client_request;
    client_request.header.to = application_get_server_pid();
    strlcpy(client_request.header.label, request, MSGLABEL_SIZE);

    message_t server_respond;

    if (payload != NULL && payload_size > 0)
    {
        message_set_payload_ptr(client_request, payload, payload_size);
    }

    if (messaging_request(&client_request, &server_respond, 100) == 0)
    {
        logger_debug("Server respond %s", server_respond.header.label);

        assert(message_is(server_respond, HIDEO_SERVER_RESULT));

        int r = (message_payload_as(server_respond, hideo_server_result_t))->value;
        RETURN_AND_SET_ERROR(r, r, -1);
    }
    else
    {
        logger_warn("Request '%s' failled: %s", request, error_to_string(error_get()));

        return -1;
    }
}

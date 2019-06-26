/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/assert.h>
#include <skift/application.h>
#include <skift/logger.h>
#include <skift/messaging.h>

static bool application_running = false;
static bool application_initialized = false;
static list_t* application_windows = NULL;
static int application_exit_value = 0;

void application_init(int argc, char** argv, const char* titles)
{
    assert(!application_initialized);
    assert(!application_running);

    application_windows = list();
    application_initialized = true;

    logger_log(LOG_INFO, "Application '%s' initialized!");
}

int application_run(void)
{
    assert(application_initialized);

    application_running = true;

    logger_log(LOG_INFO, "Entering event loop...");

    do
    {
        // Get the message

        // Is
    } while (application_running);    

    logger_log(LOG_INFO, "Event loop exited!");

    return application_exit_value;
}

void application_quit(int exit_value)
{
    assert(application_initialized);
    assert(application_running);

    application_running = false;
    application_exit_value = exit_value;

    logger_log(LOG_INFO, "Application quit!");
}

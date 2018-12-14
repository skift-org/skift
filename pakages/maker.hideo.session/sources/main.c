#include <skift/generic.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/logger.h>

int try_exec(const char* filename)
{
    sk_log(LOG_INFO, "Starting '%s'", filename);
    int process = sk_process_exec(filename, NULL);

    if (process == 0)
    {
        sk_log(LOG_WARNING, "Failed to start process: '%s'!", filename);
    }
    else
    {
        sk_log(LOG_FINE, "'%s' started!", filename);
    }

    return process;
}

int program()
{
    int compositor = try_exec("app/maker.hideo.compositor");
    int panel = try_exec("app/maker.hideo.panel");

    sk_thread_waitproc(compositor);
    sk_thread_waitproc(panel);
    
    return 0;
}

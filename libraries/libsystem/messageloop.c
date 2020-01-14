#include <libsystem/messageloop.h>
#include <libsystem/logger.h>

static messageloop_t the_messageloop = {0};

void messageloop_init(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    the_messageloop.loop_continue = false;
    the_messageloop.message_handlers = list_create();
}

void messageloop_fini(void)
{
    list_destroy(the_messageloop.message_handlers, LIST_FREE_VALUES);
}

void messageloop_register_handler(const char *message_type, messageloop_message_handler_t handler_function)
{
    messageloop_handler_t *handler = __create(messageloop_handler_t);

    handler->message_type = message_type;
    handler->handler = handler_function;

    list_pushback(the_messageloop.message_handlers, handler);
}

void messageloop_pump(bool blocking)
{
    message_t message;
    if (messaging_receive(&message, blocking) == 0)
    {
        bool message_handled = false;

        list_foreach(messageloop_handler_t, handler, the_messageloop.message_handlers)
        {
            if (strncmp(message_label(message), handler->message_type, MSGLABEL_SIZE) == 0)
            {
                handler->handler(&the_messageloop, &message, message_payload(message));

                message_handled = true;
            }
        }

        if (!message_handled)
        {
            logger_warn("No handler for message of class %s", message_label(message));
        }
    }
}

void messageloop_subscribe(const char *channel)
{
    messaging_subscribe(channel);
}

void messageloop_run(void)
{
    the_messageloop.loop_continue = true;

    while (the_messageloop.loop_continue)
    {
        messageloop_pump(true);
    }
}

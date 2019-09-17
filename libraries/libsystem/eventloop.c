#include <libsystem/eventloop.h>
#include <libsystem/logger.h>

void eventloop_delete(eventloop_t* this)
{
    list_delete(this->list, LIST_FREE_VALUES);
}

eventloop_t* eventloop()
{
    eventloop_t* this = OBJECT(eventloop);

    this->loop_continue = false;
    this->list = list();

    return this;
}

void eventloop_register_handler(eventloop_t* this, const char* message_type, eventloop_message_handler_t handler_function)
{
    eventloop_handler_t* handler = MALLOC(eventloop_handler_t);

    handler->message_type = message_type;
    handler->handler = handler_function;

    list_pushback(this->list, handler);
}

void eventloop_pump(eventloop_t* this, bool blocking)
{
    message_t message;
    if (messaging_receive(&message, blocking) == 0)
    {
        bool message_handled = false;

        list_foreach(i, this->list)
        {
            eventloop_handler_t* handler = (eventloop_handler_t*)i->value;

            if (strncmp(message_label(message), handler->message_type, MSGLABEL_SIZE) == 0)
            {
                handler->handler(this, &message, message_payload(message));

                message_handled = true;
            }
        }
        
        if (!message_handled)
        {
            logger_warn("No handler for message of class %s", message_label(message));
        }
    }
}

void eventloop_subscribe(eventloop_t* this, const char* channel)
{
    UNUSED(this);

    messaging_subscribe(channel);
}

void eventloop_run(eventloop_t* this)
{
    this->loop_continue = true;

    while(this->loop_continue)
    {
        eventloop_pump(this, true);
    }
}

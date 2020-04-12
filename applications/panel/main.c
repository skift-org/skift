#include <libsystem/cstring.h>
#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Label.h>

void widget_date_and_time_update(Widget *widget)
{
    TimeStamp timestamp = timestamp_now();
    DateTime datetime = timestamp_to_datetime(timestamp);

    char buffer[256];

    snprintf(buffer, 256, "%d:%d:%d ", datetime.hour, datetime.minute, datetime.second);

    label_set_text(widget, buffer);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("Panel", 800, 32);

    window_set_border_style(window, WINDOW_BORDER_NONE);

    window_root(window)->layout = (Layout){LAYOUT_HFLOW, 8, 0};
    window_root(window)->insets = INSETS(0, 8);

    label_create(window_root(window), "skiftOS");
    Widget *widget_date_and_time = label_create(window_root(window), "");
    widget_date_and_time->layout_attributes = LAYOUT_FILL;
    label_create(window_root(window), "user");

    timer_start(timer_create(widget_date_and_time, 500, (TimerCallback)widget_date_and_time_update));

    return application_run();
}

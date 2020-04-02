/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Stream.h>

#include <libwidget/Widgets.h>
#include <libwidget/core/Application.h>

static Window *main_window = NULL;
static Stream *random_device = NULL;

typedef struct
{
    Point start;
    Point finish;
    Color color;
} line_t;

void draw(Timer *timer)
{
    __unused(timer);

    Rectangle fb_bound = window_content_bound(main_window);

    line_t line;
    stream_read(random_device, &line, sizeof(line));

    line.start.X = fb_bound.X + abs((int)line.start.X % fb_bound.width);
    line.start.Y = fb_bound.Y + abs((int)line.start.Y % fb_bound.height);
    line.finish.X = fb_bound.X + abs((int)line.finish.X % fb_bound.width);
    line.finish.Y = fb_bound.Y + abs((int)line.finish.Y % fb_bound.height);

    painter_draw_line(main_window->painter, line.start, line.finish, line.color);

    application_blit_window(main_window, window_content_bound(main_window));
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    application_initialize(argc, argv);
    random_device = stream_open("/dev/random", OPEN_READ);

    Widget *panel0 = panel_create(NULL, RECTANGLE(100, 100, 150, 150));

    panel_create(panel0, RECTANGLE(0, 0, 50, 100));
    panel_create(panel0, RECTANGLE(50, 0, 50, 100));
    panel_create(panel0, RECTANGLE(0, 100, 100, 50));

    main_window = window_create(RECTANGLE_SIZE(250, 150));
    window_set_main_widget(main_window, panel0);

    application_dump();

    Timer *timer = timer_create(16, draw);
    timer_start(timer);

    return application_run();
}

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

#include <libwidget/Widgets.h>
#include <libwidget/core/Application.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    application_initialize(argc, argv);

    Widget *main_window = window_create(RECTANGLE_SIZE(500, 400));

    Widget *panel0 = panel_create(main_window, RECTANGLE_SIZE(0, 0));
    panel_create(panel0, RECTANGLE_SIZE(10, 10));
    panel_create(panel0, RECTANGLE_SIZE(15, 15));
    panel_create(panel0, RECTANGLE_SIZE(20, 20));
    panel_create(panel0, RECTANGLE_SIZE(30, 30));

    Widget *panel1 = panel_create(main_window, RECTANGLE_SIZE(100, 100));
    panel_create(panel1, RECTANGLE_SIZE(110, 110));
    panel_create(panel1, RECTANGLE_SIZE(115, 115));
    panel_create(panel1, RECTANGLE_SIZE(120, 120));
    panel_create(panel1, RECTANGLE_SIZE(130, 130));

    Widget *panel2 = panel_create(main_window, RECTANGLE_SIZE(200, 200));
    panel_create(panel2, RECTANGLE_SIZE(210, 210));
    panel_create(panel2, RECTANGLE_SIZE(215, 215));
    panel_create(panel2, RECTANGLE_SIZE(220, 220));
    panel_create(panel2, RECTANGLE_SIZE(230, 230));

    Widget *panel3 = panel_create(main_window, RECTANGLE_SIZE(300, 300));
    panel_create(panel3, RECTANGLE_SIZE(310, 310));
    panel_create(panel3, RECTANGLE_SIZE(315, 315));
    panel_create(panel3, RECTANGLE_SIZE(320, 320));
    panel_create(panel3, RECTANGLE_SIZE(330, 330));

    application_dump();

    return application_run();
}

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>

#include <libwidget/Widgets.h>
#include <libwidget/core/Application.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    application_initialize(argc, argv);

    Widget *main_window = window_create();

    Widget *panel0 = panel_create(main_window);
    panel_create(panel0);
    panel_create(panel0);
    panel_create(panel0);
    panel_create(panel0);

    Widget *panel1 = panel_create(main_window);
    panel_create(panel1);
    panel_create(panel1);
    panel_create(panel1);
    panel_create(panel1);

    Widget *panel2 = panel_create(main_window);
    panel_create(panel2);
    panel_create(panel2);
    panel_create(panel2);
    panel_create(panel2);

    Widget *panel3 = panel_create(main_window);
    panel_create(panel3);
    panel_create(panel3);
    panel_create(panel3);
    panel_create(panel3);

    application_dump();

    return application_run();
}

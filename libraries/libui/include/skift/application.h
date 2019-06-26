#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/window.h>

void application_init(int argc, char** argv, const char* titles);

int application_run(void);

void application_quit(int exit_value);
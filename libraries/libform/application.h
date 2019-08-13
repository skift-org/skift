#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libform/form.h>

void application_init(int argc, char **argv, const char *titles);

int application_run(void);

void application_quit(int exit_value);

int application_get_server_pid(void);

int application_send_request(const char *request, const void *payload, int payload_size);
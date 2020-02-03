#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/System.h>

SystemInfo system_get_info(void);

SystemStatus system_get_status(void);

uint system_get_ticks(void);

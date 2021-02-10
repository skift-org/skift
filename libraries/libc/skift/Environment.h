#pragma once

#include <libutils/json/Json.h>
#include <libutils/String.h>

void environment_load(const char *buffer);

json::Value &environment();

String environment_copy();

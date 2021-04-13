#pragma once

#include <libutils/String.h>
#include <libjson/Json.h>

void environment_load(const char *buffer);

Json::Value &environment();

String environment_copy();

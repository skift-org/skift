#pragma once

#include "spec.h"

#define HANOVER_NO_KSTART

#include <handover/main.h>

#undef HANOVER_NO_KSTART

void _limineEntryPoint();

volatile limine_entry_point_request reqEntryPoint = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .entry = _limineEntryPoint,
};

void _limineEntryPoint() {
    (void)entryPoint(0, {});
}

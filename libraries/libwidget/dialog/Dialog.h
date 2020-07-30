#pragma once

#include <libgraphic/Icon.h>

#define DIALOG_BUTTON_OK (1 << 0)
#define DIALOG_BUTTON_CANCEL (1 << 1)
#define DIALOG_BUTTON_YES (1 << 2)
#define DIALOG_BUTTON_NO (1 << 3)
#define DIALOG_BUTTON_CLOSED (1 << 4)

typedef unsigned DialogButton;

DialogButton dialog_message(
    RefPtr<Icon> icon,
    const char *title,
    const char *message,
    DialogButton buttons);

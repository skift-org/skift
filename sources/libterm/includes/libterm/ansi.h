#pragma once

#define ANSI_ESC '\033'

typedef enum
{
    ANSI_UNKNOW,

    /* Cursor operations */
    ANSI_CURSOR_SET,

    ANSI_CURSOR_UP,
    ANSI_CURSOR_DOWN,
    
    ANSI_CURSOR_FORWARD,
    ANSI_CURSOR_BACKWARD,

    ANSI_CURSOR_SAVE,
    ANSI_CURSOR_RESTOR,

    /* screen operations */
    ANSI_CLEAR,
    ANSI_CLEAR_LINE,
    ANSI_SET_MODE,
} ansitype_t;

int ansi_len(const char * esc);
ansitype_t ansi_get_type(const char * esc);
int ansi_value_count(const char * esc);
int ansi_value(const char * esc, int index);

#pragma once

#include <__libc__.h>

__BEGIN_HEADER

int isalnum(int c);
int isalpha(int c);
int isdigit(int c);
int islower(int c);
int isprint(int c);
int isgraph(int c);
int iscntrl(int c);
int isgraph(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

int isascii(int c);

int tolower(int c);
int toupper(int c);

#define _U 01
#define _L 02
#define _N 04
#define _S 010
#define _P 020
#define _C 040
#define _X 0100
#define _B 0200

extern char _ctype_[256];

__END_HEADER

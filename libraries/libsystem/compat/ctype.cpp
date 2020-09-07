#include <ctype.h>

extern "C"
{
    int isprint(int c)
    {
        return (c >= 0x20 && c <= 0x7e) ? 1 : 0;
    }

    int isalpha(int c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0;
    }

    int isdigit(int c)
    {
        return (c >= '0' && c <= '9' ? 1 : 0);
    }

    int isalnum(int c)
    {
        return isalpha(c) || isdigit(c);
    }
}

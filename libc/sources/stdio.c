#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int printf(const char *format, ...)
{
    va_list va;
    va_start(va, format);

    int result = vprintf(format, va);

    va_end(va);

    return result;
}

int vprintf(const char *format, va_list va)
{
    char buffer[strlen(format) + 128];
    int result = vsprintf(buffer, format, va);
    puts(buffer);

    return result;
}

int sprintf(char *str, const char *format, ...)
{
    va_list va;
    va_start(va, format);

    int result = vsprintf(str, format, va);

    va_end(va);

    return result;
}

int vsprintf(char *str, const char *format, va_list va)
{
    str[0] = '\0';

    bool wait_for_format = false;

    for (unsigned int i = 0; format[i]; i++)
    {
        char c = format[i];

        if (c == '%' && !wait_for_format)
        {
            wait_for_format = true;
        }
        else if (wait_for_format)
        {
            char temp[32];
            switch (c)
            {
                case 'b':
                    itos(va_arg(va, unsigned int), temp, 2);
                    strcat(str, "0b");
                    strcat(str, temp);
                    break;

                case 'i':
                case 'd':
                    {
                        bool isneg = false;
                        int value = va_arg(va, unsigned int);
                        unsigned int uvalue = 0;
                        
                        if (value < 0)
                        {
                            isneg = true;
                            uvalue = 0 - value;
                        }
                        else
                        {
                            uvalue = value;
                        }

                        if (isneg)
                        {
                            strcat(str, "-");
                        }

                        itos(uvalue, temp, 10);
                        strcat(str, temp);
                    }
                    break;
                case 'u':
                    itos(va_arg(va, unsigned int), temp, 10);
                    strcat(str, temp);
                    break;

                case 'x':
                    itos(va_arg(va, unsigned int), temp, 16);
                    strcat(str, temp);
                    break;

                case 'c':
                    strapd(str, va_arg(va, char));
                    break;

                case 's':
                    strcat(str, va_arg(va, const char *));
                    break;

                case '%':
                    strapd(str, '%');
                    break;

                default:
                    strapd(str, '%');
                    strapd(str, c);
                    break;
            }

            wait_for_format = false;
        }
        else
        {
            strapd(str, c);
        }
    }

    return strlen(str);
}
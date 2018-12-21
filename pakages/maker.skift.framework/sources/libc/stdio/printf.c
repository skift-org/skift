#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <skift/formater.h>

int vsnprintf(char* s, size_t n, const char * fmt, va_list va)
{
    printf_info_t _info = 
    {
        .output = s,
        .output_size = n,
        .format = fmt,

        .state = PFSTATE_ESC,
    };

    printf_info_t* info = &_info;

    PEEK();

    while(1)
    {
        switch (info->state)
        {
            case PFSTATE_ESC:
                if (info->c == '%')
                {
                    info->state = PFSTATE_PARSE;
                }
                else 
                {
                    APPEND(info->c);
                }

                PEEK();
                break;
        
            case PFSTATE_PARSE:
                if (info->c == '0')
                {
                    info->padding = '0';
                }
                else if (info->c == '-')
                {
                    info->align = PFALIGN_LEFT;
                }
                else if (isdigit(info->c))
                {
                    info->state = PFSTATE_FORMAT_LENGHT;
                }
                else if (isalpha(info->c))
                {
                    info->state = PFSTATE_FINALIZE;
                }
                else
                {
                    PEEK();
                }
                break;

            case PFSTATE_FORMAT_LENGHT:
                if (isdigit(info->c))
                {
                    info->lenght*=10;
                    info->lenght += info->c - '0';
                    
                    PEEK();
                }
                else if (isalpha(info->c))
                {
                    info->state = PFSTATE_PARSE;
                }
                else
                {
                    info->state = PFSTATE_ESC;
                }
                break;

            case PFSTATE_FINALIZE:
                sk_formater_format(info, info->c, &va);
                info->lenght = 0;
                info->state = PFSTATE_ESC;

                PEEK();
                break;
            
            default:
                break;
        }
    }
}

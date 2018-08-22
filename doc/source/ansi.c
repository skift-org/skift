#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ANSI_ESC '\033'

void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

int ansi_len(const char * esc)
{
    int i = 0;

    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        i += 2;

        while (esc[i] == ';' || isdigit(esc[i])) i++;
    
        if (isalpha(esc[i]))
        {
            i++;
        }

    }

    return i;
}

int ansi_value_count(const char *esc)
{
    int count = 0;
    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        int wait_for_digit = 1;

        for(size_t i = 2; isdigit(esc[i]) || esc[i] == ';'; i++)
        {
            if (wait_for_digit && isdigit(esc[i]))
            {
                wait_for_digit = 0;
                count++;
            }

            if (esc[i] == ';')
            {
                wait_for_digit = 1;
            }
        }
    }

    return count;
}

int ansi_value(const char *esc, int index)
{
    char buffer[12];
    buffer[0] = '\0';

    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        esc += 2;

        for(size_t i = 0; isdigit(esc[i]) || esc[i] == ';'; i++)
        {
            char c = esc[i];

            if (c == ';')
            {
                index--;
            }
            else if (index == 0)
            {
                append(buffer, c);
            }
        }
        
        return atoi(buffer);
    }

    return 0;
}

void test(const char * esc)
{
    printf("esc: %s = COUNT=%i LEN=%i\n", esc + 1, ansi_value_count(esc), ansi_len(esc));
    
    for(size_t i = 0; i < ansi_value_count(esc); i++)
    {
        printf("val: %i\n", ansi_value(esc, i));
    }
    
}

int main()
{
    test("\033[m sdhfjkqsdhflkj");
    test("\033[43m sdhfjkqsdhflkj");
    test("\033[43;42m qsdfdskjlfhk");
    test("\033[43;42;24m fqsjmldfjsl");

    return 0;
}
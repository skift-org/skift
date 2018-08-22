
#include <stdio.h>
#include <string.h>

void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

int path_read(const char * path, int index, char * buffer)
{
    int current_index = 0;
    buffer[0] = '\0';
    
    if (path[0] == '/') path++;

    for (int i = 0; path[i]; i++)
    {
        char c = path[i];
        
        if (current_index == index)
        {
            if (c == '/')
            {
                break;
            }
            else
            {
                append(buffer, c);
            }
        }

        if ( c == '/')
        {            
            current_index++;
            buffer[0] = '\0';
        }
    }
    
    return (current_index == index && strlen(buffer)) ? 1 : 0;
}

void path_test(const char * path)
{
    char buffer[128];
    
    printf("%s\n", path);
    
    for (int i = 0; path_read(path, i, buffer); i++) 
    {
        printf("%s, ", buffer);
    }
    
    printf("\nDONE\n\n");
}

int main()
{
    path_test("user");
    path_test("user/");
    path_test("/user");
    path_test("/user/");

    path_test("user/document/project/skift");
    path_test("/user/document/project/skift");
    path_test("user/document/project/skift/");
    path_test("/user/document/project/skift/");
    
    return 0;
}
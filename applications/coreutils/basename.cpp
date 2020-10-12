#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>

/*
    Implementation of basename shell command.
    basename(): returns the component following the final '/', discarding trailing '/' characters.
*/


void basename(char *path) 
{
    /* If the string is empty or NULL, output should be "." */
    if (path == NULL || *path == '\0') {
        printf(".");
        return;
    }

    /* Removing trailing slashes */
    char *endp = path + strlen(path) - 1;
    while (endp > path && *endp == '/') 
        endp--;

    /* Case wherein path only consists of slashes */
    if (endp == path && *endp == '/') {
        printf("/");
        return;
    }

    /* Finding the base start */
    char *startp = endp;
    while(startp > path && *(startp-1) != '/')
        startp--;

    while (startp <= endp) {
        printf("%c", *startp);
        startp++;
    }
}

int main(int argc, char *argv[]) 
{    
    if (argc <= 1) 
    {
        stream_format(err_stream, "usage: basename path\n");
        return 0;
    }

    char *path = argv[1];

    basename(path);

    return 0;
}